#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
using namespace std;

typedef vector<vector<float>> Matrix;

mt19937 gen(random_device{}());
const int h=8,w=8;
const vector<int> layer_sizes={h*w,128,64,10};
const int L=layer_sizes.size()-1;
const float lr=0.1f;
const int epochs=100;

uint32_t read_big_endian(std::ifstream& file) {
    uint32_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    // 转换为小端序 (假设运行环境为小端序，如x86)
    return ((value & 0xFF) << 24) | ((value & 0xFF00) << 8) |
           ((value & 0xFF0000) >> 8) | ((value & 0xFF000000) >> 24);
}

// 将28x28图像降采样为8x8并二值化
std::vector<float> downsample_and_binarize(const std::vector<uint8_t>& image_28x28) {
    std::vector<float> image_8x8(h * w, 0.0f);

    for (int by=0; by<h;++by) {
        for (int bx=0; bx<w;++bx) {
            int sum=0;
            int count=0;
            const int y_begin=by*28 / h;
            const int y_end=(by+1)*28 / h;
            const int x_begin=bx*28 / w;
            const int x_end=(bx+1)*28 / w;
            // 按比例划分像素区域，保证8个分区都非空且覆盖整张图。
            for (int y=y_begin; y<y_end;++y) {
                for (int x=x_begin; x<x_end;++x) {
                    sum += image_28x28[y*28+x];
                    count++;
                }
            }
            float avg=static_cast<float>(sum) / count;
            // image_8x8[by*8+bx]=(avg>127.0f)?0.9f:-1.0f;
            image_8x8[by*w+bx]=avg/255.0f-0.1f;
        }
    }
    return image_8x8;
}

// 加载MNIST数据并转换为8x8二值化格式
void load_mnist_8x8(const std::string& image_path, const std::string& label_path,
                    std::vector<std::vector<float>>& images,
                    std::vector<int>& labels) {
    std::ifstream img_file(image_path, std::ios::binary);
    std::ifstream lbl_file(label_path, std::ios::binary);

    if (!img_file.is_open() || !lbl_file.is_open()) {
        std::cerr << "无法打开文件!" << std::endl;
        return;
    }

    // 读取图像文件头
    uint32_t magic=read_big_endian(img_file);
    if (magic != 2051) { std::cerr << "图像文件魔数错误!" << std::endl; return; }
    uint32_t num_images=read_big_endian(img_file);
    uint32_t rows=read_big_endian(img_file);
    uint32_t cols=read_big_endian(img_file);

    // 读取标签文件头
    uint32_t label_magic=read_big_endian(lbl_file);
    if (label_magic != 2049) { std::cerr << "标签文件魔数错误!" << std::endl; return; }
    uint32_t num_labels=read_big_endian(lbl_file);

    if (num_images != num_labels) { std::cerr << "图像与标签数量不匹配!" << std::endl; return; }

    images.clear();
    labels.clear();
    images.reserve(num_images);
    labels.reserve(num_labels);

    std::vector<uint8_t> buffer(rows*cols);

    for (uint32_t i=0; i<num_images;++i) {
        // 读取一张28x28的原始图像
        img_file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        // 降采样并二值化
        images.push_back(downsample_and_binarize(buffer));
        // 读取对应的标签
        uint8_t label;
        lbl_file.read(reinterpret_cast<char*>(&label), 1);
        labels.push_back(static_cast<int>(label));
    }

    std::cout << "加载完成: " << images.size() << " 张图像, "
              << labels.size() << " 个标签" << std::endl;
}

void Init_images(vector<vector<float>>&train_images, vector<int> &train_labels) {
    load_mnist_8x8("train-images.idx3-ubyte", "train-labels.idx1-ubyte",
                   train_images, train_labels);
}

inline Matrix new_matrix(int n,int m){
    return Matrix(n,vector<float>(m,0.0));
}
void he_init(Matrix &a, int fan_in) {
    double std_dev=sqrt(2.0 / fan_in);
    normal_distribution<double> dist(0.0, std_dev);
    for (auto &row:a)
        for (auto &val:row)
            val=dist(gen);
}
void zero_inplace(Matrix &a){
    for(auto &row : a)
        fill(row.begin(), row.end(), 0.0f);
}
void mul(const Matrix &a,const Matrix &b,Matrix &c){
    const int n=a.size(),m=a[0].size(),p=b[0].size();
    zero_inplace(c);
    for(int i=0;i<n;++i)
        for(int k=0;k<m;++k){
            float aik=a[i][k];
            for(int j=0;j<p;++j)
                c[i][j]+=aik*b[k][j];
        }
}
void add(const Matrix &a,const Matrix &b,Matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]+b[i][j];
}
void mul_add(const Matrix &a,const Matrix &b,const Matrix &c,Matrix &d){
    const int n=a.size(),m=a[0].size(),p=b[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<p;++j)
            d[i][j]=c[i][j];
    for(int i=0;i<n;++i)
        for(int k=0;k<m;++k){
            float aik=a[i][k];
            for(int j=0;j<p;++j)
                d[i][j]+=aik*b[k][j];
        }
}
void sub(const Matrix &a,const Matrix &b,Matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]-b[i][j];
}
Matrix scalar_mul(const Matrix &a,float b){
    const int n=a.size(),m=a[0].size();
    Matrix c=new_matrix(n,m);
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]*b;
    return c;
}
void sub_scalar_mul(const Matrix &a,const Matrix &b,float c,Matrix &d){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            d[i][j]=a[i][j]-b[i][j]*c;
}
void transpose(const Matrix &a, Matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[j][i]=a[i][j];
}
Matrix LeakyReLU(const Matrix &a){
    const int n=a.size(),m=a[0].size();
    Matrix c=new_matrix(n,m);
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]>0?a[i][j]:0.01f*a[i][j];
    return c;
}
Matrix LeakyReLU_derivative(const Matrix &a){
    const int n=a.size(),m=a[0].size();
    Matrix c=new_matrix(n,m);
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]>0?1.0f:0.01f;
    return c;
}
void Hadamard(const Matrix &a,const Matrix &b,Matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]*b[i][j];
}
Matrix softmax(const Matrix &z) {
    const int m=z[0].size();
    float max_val=z[0][0];
    for(int j=1;j<m;++j)
        max_val=max(max_val,z[0][j]);
    float sum=0;
    Matrix res=new_matrix(1, m);
    for(int j=0;j<m;++j){
        res[0][j]=exp(z[0][j]-max_val);
        sum+=res[0][j];
    }
    for(int j=0;j<m;++j)
        res[0][j]/=sum;
    return res;
}
Matrix augment(const Matrix &x){
    Matrix aug = x;
    int r = uniform_int_distribution<int>(0, 3)(gen);
    if(r == 0){  // 随机像素翻转
        int p = uniform_int_distribution<int>(0, x[0].size()-1)(gen);
        aug[0][p] = (aug[0][p] > 0.5f) ? -0.1f : 0.9f;
    } else if(r == 1){  // 左移
        for(int i=0;i<h;++i)
            for(int j=0;j<w-1;++j)
                aug[0][i*w+j] = aug[0][i*w+j+1];
    } else if(r == 2){  // 右移
        for(int i=0;i<h;++i)
            for(int j=w-1;j>0;--j)
                aug[0][i*w+j] = aug[0][i*w+j-1];
    } else {  // 边缘模糊
        Matrix blur = aug;
        for(int i=0;i<h;++i)
            for(int j=0;j<w;++j){
                float v = aug[0][i*w+j];
                if(i>0) v += aug[0][(i-1)*w+j] * 0.15f;
                if(i<h-1) v += aug[0][(i+1)*w+j] * 0.15f;
                if(j>0) v += aug[0][i*w+j-1] * 0.15f;
                if(j<w-1) v += aug[0][i*w+j+1] * 0.15f;
                blur[0][i*w+j] = v;
            }
        aug = blur;
    }
    return aug;
}
vector<Matrix> W(L),B(L);
vector<Matrix> Z(L),A(L+1),dZ(L),dW(L);
vector<Matrix> sum_dW(L),sum_dB(L),tA(L),tW(L),dA(L);
void init_network(){
    W.resize(L);
    B.resize(L);
    for(int l=0;l<L;++l){
        W[l]=new_matrix(layer_sizes[l],layer_sizes[l+1]);
        B[l]=new_matrix(1,layer_sizes[l+1]);
        he_init(W[l],layer_sizes[l]);
        // B 保持 0
    }
}
void init_buffers(){
    for(int l=0; l<L;++l){
        Z[l]=new_matrix(1,layer_sizes[l+1]);
        dZ[l]=new_matrix(1,layer_sizes[l+1]);
        dW[l]=new_matrix(layer_sizes[l],layer_sizes[l+1]);
        sum_dW[l]=new_matrix(layer_sizes[l],layer_sizes[l+1]);
        sum_dB[l]=new_matrix(1,layer_sizes[l+1]);
        tA[l]=new_matrix(layer_sizes[l],1);
        tW[l]=new_matrix(layer_sizes[l+1],layer_sizes[l]);
        dA[l]=new_matrix(1,layer_sizes[l]);
    }
    for(int l=0;l<=L;++l)
        A[l]=new_matrix(1,layer_sizes[l]);
}
void forward(const Matrix &x){
    A[0]=x;
    for(int l=0;l<L;++l){
        mul_add(A[l],W[l],B[l],Z[l]);
        A[l+1]=(l<L-1?LeakyReLU:softmax)(Z[l]);
    }
}
void backward(const Matrix &y){
    // 输出层误差：dZ[L-1]=A[L] - y
    sub(A[L],y,dZ[L-1]);

    // 从最后一层往第一层回传
    for(int l=L-1;l>=0;--l){
        // 权重梯度：dW[l]=A[l]^T*dZ[l]
        transpose(A[l],tA[l]);
        mul(tA[l],dZ[l],dW[l]);

        // 偏置梯度：dB[l]=dZ[l]
        // 直接累加，不用 db2=dz2 复制

        // 如果不是第一层，继续回传
        if(l>0){
            // dA=dZ[l]*W[l]^T
            transpose(W[l],tW[l]);
            mul(dZ[l],tW[l],dA[l]);

            // dZ[l-1]=dA ⊙ LeakyReLU'(Z[l-1])
            Hadamard(dA[l],LeakyReLU_derivative(Z[l-1]),dZ[l-1]);
        }
    }
}
void update_params(float scale){
    for(int l=0;l<L;++l){
        sub_scalar_mul(W[l],sum_dW[l],scale,W[l]);
        sub_scalar_mul(B[l],sum_dB[l],scale,B[l]);
    }
}
void training(){
    vector<Matrix> X;
    vector<Matrix> Y;
    vector<vector<float>>train_images;
    vector<int> train_labels;
    Init_images(train_images, train_labels);
    for(const auto &img:train_images)
        X.push_back(vector<vector<float>>(1,img));
    for(const auto &label:train_labels){
        Y.push_back(vector<vector<float>>(1,vector<float>(10,0.0)));
        Y.back()[0][label]=1.0;
    }
    init_network();
    init_buffers();
    const int batch_size=64;
    const int num_batches=X.size()/batch_size;
    vector<int> idx(X.size());
    iota(idx.begin(),idx.end(),0);
    clock_t start=clock();
    for(int epoch=0;epoch<epochs;++epoch){
        float total_loss=0;
        int cnt=0;
        shuffle(idx.begin(),idx.end(),gen);
        for(int b=0;b<num_batches;++b){
            for(int l=0;l<L;++l){
                zero_inplace(sum_dW[l]);
                zero_inplace(sum_dB[l]);
            }
            for(int k=0,pos;k<batch_size&&(pos=b*batch_size+k)<(int)idx.size();++k){
                const Matrix &x=augment(X[idx[pos]]),&y=Y[idx[pos]];
                forward(x);
                backward(y);
                for(int l=0;l<L;++l){
                    add(sum_dW[l],dW[l],sum_dW[l]);
                    add(sum_dB[l],dZ[l],sum_dB[l]);  // dB[l]=dZ[l]
                }
                // loss
                for(int j=0;j<layer_sizes[L];++j)
                    total_loss-=y[0][j]*log(A[L][0][j]+1e-12);
                cnt++;
            }
            update_params(lr/batch_size);  // 更新参数
        }
        // if(epoch%30==0)
        cout<<"Epoch "<<epoch<<", Loss: "<<total_loss/cnt<<endl;
    }
    cout<<"训练结束"<<endl;
    double elapsed_ms=(double)(clock()-start)/CLOCKS_PER_SEC*1000.0;
    cout<<"运行时间："<<elapsed_ms<<" ms"<<endl;
}
int predict(const Matrix &input){
    forward(input);
    return max_element(A[L][0].begin(), A[L][0].end()) - A[L][0].begin();
}
void test(){
    cout<<"测试集:"<<endl;
    vector<vector<float>> test_images;
    vector<int> test_labels;
    load_mnist_8x8("t10k-images.idx3-ubyte", "t10k-labels.idx1-ubyte",
                test_images, test_labels);
    int correct=0;
    float test_loss=0;
    for(int i=0;i<(int)test_images.size();++i){
        const Matrix x(1,test_images[i]);
        int predicted_label=predict(x);
        if(predicted_label==test_labels[i])
            correct++;
        for(int j=0;j<layer_sizes[L];++j)
            test_loss -= (test_labels[i]==j?1.0f:0.0f) * log(A[L][0][j] + 1e-12);
        // cout<<"样本 "<<i<<": True label: "<<test_labels[i]<<", Predicted label: "<<predicted_label<<endl;
    }
    cout<<"正确率: "<<correct<<"/"<<test_images.size()<<endl;
    cout<<"测试损失: "<<test_loss/test_images.size()<<endl;
}
void save_model(const string &path){
    ofstream fout(path);
    fout << layer_sizes.size() << "\n";
    for(int s:layer_sizes) fout << s << " ";
    fout << "\n";

    for(int l=0; l<L;++l){
        auto write=[&](const Matrix &m){
            fout << m.size() << " " << m[0].size() << "\n";
            for(auto &row:m){
                for(int j=0; j<(int)row.size();++j){
                    if(j) fout << " ";
                    fout << row[j];
                }
                fout << "\n";
            }
        };
        write(W[l]);
        write(B[l]);
    }
}
bool load_model(const string &path){
    ifstream fin(path);
    if(!fin) return false;
    int layers;
    fin>>layers;
    vector<int> sizes(layers);
    for(int i=0; i<layers;++i) fin >> sizes[i];
    if(sizes!=layer_sizes){
        cerr << "模型结构与当前网络不匹配!" << endl;
        return false;
    }
    for(int l=0;l<L;++l){
        auto read=[&](Matrix &m){
            int n,cols;
            fin>>n>>cols;
            m.assign(n,vector<float>(cols));
            for(int i=0;i<n;++i)
                for(int j=0;j<cols;++j)
                    fin>>m[i][j];
        };
        read(W[l]);
        read(B[l]);
    }
    return true;
}
void center_digit(Matrix &input){
    int min_i=h, max_i=-1, min_j=w, max_j=-1;
    for(int i=0;i<h;++i)
        for(int j=0;j<w;++j)
            if(input[i][j] > 0.5f){
                min_i = min(min_i, i);
                max_i = max(max_i, i);
                min_j = min(min_j, j);
                max_j = max(max_j, j);
            }
    if(max_i < 0) return;  // 空图
    int ci = (min_i + max_i) / 2;
    int cj = (min_j + max_j) / 2;
    int di = h/2 - ci;
    int dj = w/2 - cj;
    Matrix shifted = new_matrix(h, w);
    for(int i=0;i<h;++i)
        for(int j=0;j<w;++j){
            shifted[i][j] = -0.1f;
            int ni = i - di, nj = j - dj;
            if(ni>=0 && ni<h && nj>=0 && nj<w)
                shifted[i][j] = input[ni][nj];
        }
    input = shifted;
}
void Setpos(int x,int y){
	COORD pos;
	pos.X=x;
	pos.Y=y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
}
int main(){
    SetConsoleOutputCP(CP_UTF8);
    if(!load_model("model.txt")) {
        cout << "未找到模型文件，开始训练..." << endl;
        training();
        test();
        save_model("model.txt");
    }else{
        init_buffers();
        cout<<"是否重新训练模型？(y/n): ";
        char choice;
        cin>>choice;
        if(choice=='y'||choice=='Y'){
            cout<<"重新训练模型..."<<endl;
            training();
            test();
            save_model("model.txt");
        }
        cout<<"是否测试模型？(y/n): ";
        cin>>choice;
        if(choice=='y'||choice=='Y')
            test();
    }
    Sleep(2000);
    while(1){
        system("cls");
        cout<<"手写数字识别:"<<endl;
        cout << "+" << string(w, '-') << "+\n";
        for(int i=0;i<h;++i)
            cout << "|" << string(w, ' ') << "|\n";
        cout << "+" << string(w, '-') << "+\n";
        Matrix input=new_matrix(h,w);
        for(int i=0;i<h;++i)
            for(int j=0;j<w;++j)
                input[i][j]=-0.1f;
        Setpos(1,2);
        for(int i=0;i<h;++i){
            Setpos(1,i+2);
            for(int j=0;j<w;++j)
                cout<<(input[i][j]>0.5f?"#":" ");
        }
        Setpos(0,3+h);
        cout<<"请在"<<w<<"x"<<h<<"的网格中绘制数字，'#'表示像素点，空格表示背景"<<endl;
        cout<<"wasd移动，空格切换像素，回车预测，r重置，ESC退出"<<endl;
        int x=0,y=0,shift=1;
        Setpos(x+1,y+2);
        while(1){
            int ch=_getch();
            Setpos(x+1,y+2);
            cout<<(input[y][x]>0.5f?"#":" ");
            switch(ch){
                case 119://w
                    if(y>0) y--;
                    break;
                case 115://s
                    if(y<h-1) y++;
                    break;
                case 97://a
                    if(x>0) x--;
                    break;
                case 100://d
                    if(x<w-1) x++;
                    break;
                case 32://space
                    shift^=1;
                    break;
                case 114://r
                    for(int i=0;i<h;++i)
                        for(int j=0;j<w;++j)
                            input[i][j]=-0.1f;
                    for(int i=0;i<h;++i){
                        Setpos(1,i+2);
                        for(int j=0;j<w;++j)
                            cout<<(input[i][j]>0.5f?"#":" ");
                    }
                    shift=0;
                    x=y=0;
                    break;
                case 13://enter
                {
                    Matrix X=new_matrix(1,layer_sizes[0]);
                    center_digit(input);
                    for(int i=0;i<h;++i)
                        for(int j=0;j<w;++j){
                            X[0][i*w+j]=input[i][j]*0.6f;
                            if(i>0) X[0][i*w+j]+=input[i-1][j]*0.1f;
                            if(i<h-1) X[0][i*w+j]+=input[i+1][j]*0.1f;
                            if(j>0) X[0][i*w+j]+=input[i][j-1]*0.1f;
                            if(j<w-1) X[0][i*w+j]+=input[i][j+1]*0.1f;
                        }
                    int predicted_label=predict(X);
                    Setpos(0,5+h);
                    cout<<"预测结果: "<<predicted_label<<endl;
                    _getch();
                    goto end;
                }
                break;
                case 27://esc
                    return 0;
            }
            Setpos(x+1,y+2);
            input[y][x]=shift?-0.1f:0.9f;
            cout<<(input[y][x]>0.5f?"#":" ");
            continue;
            end:break;
        }
    }
    return 0;
}
