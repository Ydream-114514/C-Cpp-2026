#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
using namespace std;

typedef vector<vector<float>> Matrix;
inline Matrix new_matrix(int n,int m){
    return Matrix(n,vector<float>(m,0.0));
}
mt19937 gen(random_device{}());
// mt19937 gen(time(0));
const int h=8,w=8;
const vector<int> layer_sizes={h*w,96,32,10};
const int L=layer_sizes.size()-1;
const int epochs=200;
const float p=0.1;

uint32_t read_big_endian(ifstream& file) {
    uint32_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    // 转换为小端序 (假设运行环境为小端序，如x86)
    return ((value & 0xFF)<<24) | ((value & 0xFF00)<<8) |
           ((value & 0xFF0000) >> 8) | ((value & 0xFF000000) >> 24);
}

// 将28x28图像降采样为8x8并二值化
vector<float> downsample_and_binarize(const vector<uint8_t>& image_28x28) {
    vector<float> image_8x8(h * w, 0.0f);

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
void load_mnist_8x8(const string& image_path, const string& label_path,
                    vector<vector<float>>& images,
                    vector<int>& labels) {
    ifstream img_file(image_path, ios::binary);
    ifstream lbl_file(label_path, ios::binary);

    if (!img_file.is_open() || !lbl_file.is_open()) {
        cerr<<"无法打开文件!"<<endl;
        return;
    }

    // 读取图像文件头
    uint32_t magic=read_big_endian(img_file);
    if (magic != 2051) { cerr<<"图像文件魔数错误!"<<endl; return; }
    uint32_t num_images=read_big_endian(img_file);
    uint32_t rows=read_big_endian(img_file);
    uint32_t cols=read_big_endian(img_file);

    // 读取标签文件头
    uint32_t label_magic=read_big_endian(lbl_file);
    if (label_magic != 2049) { cerr<<"标签文件魔数错误!"<<endl; return; }
    uint32_t num_labels=read_big_endian(lbl_file);

    if (num_images != num_labels) { cerr<<"图像与标签数量不匹配!"<<endl; return; }

    images.clear();
    labels.clear();
    images.reserve(num_images);
    labels.reserve(num_labels);

    vector<uint8_t> buffer(rows*cols);

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

    cout<<"加载完成: "<<images.size()<<" 张图像, "<<labels.size()<<" 个标签"<<endl;
}

void load_MNIST(const string &image_path,const string &lable_path,vector<Matrix> &X,vector<Matrix> &Y) {
    vector<vector<float>>train_images;
    vector<int> train_labels;
    load_mnist_8x8(image_path,lable_path,train_images,train_labels);
    for(const auto &img:train_images)
        X.push_back(vector<vector<float>>(1,img));
    for(const auto &label:train_labels){
        Y.push_back(vector<vector<float>>(1,vector<float>(10,0.0)));
        Y.back()[0][label]=1.0;
    }
}

void load_optdigits(const string& filename,vector<Matrix> &X,vector<Matrix> &Y) {
    ifstream file(filename);
    if (!file.is_open()){
        cerr<<"无法打开文件: "<<filename<<endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        Matrix x=new_matrix(1,h*w),y=new_matrix(1,10);
        for (int i=0; i<64; ++i) {
            getline(ss, value, ',');
            // x[0][i]=stoi(value)>8?1.f:0.f;
            x[0][i]=stoi(value)/16.f;
        }
        X.push_back(x);
        getline(ss, value, ',');
        y[0][stoi(value)]=1;
        Y.push_back(y);
    }
    file.close();
    cout<<"Optdigits 加载完成: "<<X.size()<<" 个样本"<<endl;
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
void scalar_mul(const Matrix &a,float b,Matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]*b;
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
void LeakyReLU(const Matrix &a,Matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]>0?a[i][j]:0.01f*a[i][j];
}
void LeakyReLU_hadamard(const Matrix &a,const Matrix &b,Matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=(a[i][j]>0?a[i][j]:0.01f*a[i][j])*b[i][j];
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
    Matrix aug=x;
    int r=uniform_int_distribution<int>(0, 2)(gen);
    if(r==0){  // 随机像素翻转
        int p=uniform_int_distribution<int>(0, x[0].size()-1)(gen);
        aug[0][p]=1.f-aug[0][p];
    }else if(r==1){  // 左移
        for(int i=0;i<h;++i){
            for(int j=0;j<w-1;++j)
                aug[0][i*w+j]=aug[0][i*w+j+1];
            aug[0][i*w+w-1] = 0.f;
        }
    }else{  // 右移
        for(int i=0;i<h;++i){
            for(int j=w-1;j>0;--j)
                aug[0][i*w+j]=aug[0][i*w+j-1];
            aug[0][i*w] = 0.f;
        }
    }
    return aug;
}
vector<Matrix> W(L),B(L);
vector<Matrix> Z(L),A(L+1),dZ(L),dW(L);
vector<Matrix> sum_dW(L),sum_dB(L),tA(L),tW(L),dA(L);
vector<Matrix> vW(L), vB(L);
vector<Matrix> D(L);
float momentum=0.9f;
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
    for(int l=0;l<L;++l){
        Z[l]=new_matrix(1,layer_sizes[l+1]);
        dZ[l]=new_matrix(1,layer_sizes[l+1]);
        dW[l]=new_matrix(layer_sizes[l],layer_sizes[l+1]);
        sum_dW[l]=new_matrix(layer_sizes[l],layer_sizes[l+1]);
        sum_dB[l]=new_matrix(1,layer_sizes[l+1]);
        tA[l]=new_matrix(layer_sizes[l],1);
        tW[l]=new_matrix(layer_sizes[l+1],layer_sizes[l]);
        dA[l]=new_matrix(1,layer_sizes[l]);
        vW[l]=new_matrix(layer_sizes[l],layer_sizes[l+1]);
        vB[l]=new_matrix(1,layer_sizes[l+1]);
        D[l]=new_matrix(1,layer_sizes[l+1]);
    }
    for(int l=0;l<=L;++l)
        A[l]=new_matrix(1,layer_sizes[l]);
}
void forward(const Matrix &x,bool mode=0){
    A[0]=x;
    uniform_real_distribution<float> dist(0,1);
    for(int l=0;l<L;++l){
        mul_add(A[l],W[l],B[l],Z[l]);
        if(l<L-1){
            if(mode){
                for(int i=0;i<layer_sizes[l+1];++i)
                    D[l][0][i]=(dist(gen)>p)/(1-p);
                LeakyReLU_hadamard(Z[l],D[l],A[l+1]);
            }else LeakyReLU(Z[l],A[l+1]);
        }else A[l+1]=softmax(Z[l]);
    }
}
void backward(const Matrix &y){
    // 输出层误差：dZ[L-1]=A[L]-y
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
            if(l-1<L-1) Hadamard(dA[l],D[l-1],dA[l]);
            Hadamard(dA[l],LeakyReLU_derivative(Z[l-1]),dZ[l-1]);
        }
    }
}
void update_params(float scale){
    for(int l=0; l<L; ++l){
        for(int i=0; i<layer_sizes[l]; ++i)
            for(int j=0; j<layer_sizes[l+1]; ++j){
                vW[l][i][j]=momentum * vW[l][i][j]+sum_dW[l][i][j] * scale;
                W[l][i][j] -= vW[l][i][j];
            }
        for(int j=0; j<layer_sizes[l+1]; ++j){
            vB[l][0][j]=momentum * vB[l][0][j]+sum_dB[l][0][j] * scale;
            B[l][0][j] -= vB[l][0][j];
        }
    }
}
void plot_loss_axis(const vector<float> &loss){
    const int W=60, H=15;
    float mn=*min_element(loss.begin(), loss.end());
    float mx=*max_element(loss.begin(), loss.end());
    if(mx-mn<1e-6f) mx=mn+1e-6f;

    vector<float> sampled(W);
    for(int i=0;i<W;++i)
        sampled[i]=loss[i * (loss.size()-1) / (W-1)];

    vector<string> canvas(H, string(W, ' '));
    for(int i=0;i<W;++i){
        int y=H-1-(int)((sampled[i]-mn)/(mx-mn)*(H-1));
        canvas[y][i]='*';
    }

    // Y 轴刻度
    cout<<fixed<<setprecision(2);
    for(int i=0;i<H;++i){
        float val=mx-(mx-mn) * i / (H-1);
        cout<<setw(6)<<val<<" |"<<canvas[i]<<"\n";
    }
    cout<<"       +"<<string(W, '-')<<"\n";
    cout<<"         epoch 0";
    // 中间和末尾的 epoch 标签
    for(int i=0;i<W-15;++i) cout<<' ';
    cout<<"epoch "<<loss.size()-1<<"\n";
}
void training(){
    vector<Matrix> X,Y;
    // load_MNIST("train-images.idx3-ubyte","train-labels.idx1-ubyte",X,Y);
    load_optdigits("optdigits.tra",X,Y);
    init_network();
    init_buffers();
    const int batch_size=64;
    const int num_batches=X.size()/batch_size;
    vector<int> idx(X.size());
    iota(idx.begin(),idx.end(),0);
    vector<float> Loss;
    ofstream Log("loss.csv");
    Log<<"epoch,loss\n";
    clock_t start=clock();
    float lr=0.1f;
    for(int epoch=0;epoch<epochs;++epoch){
        if(epoch&&epoch%50==0) lr*=0.5f;
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
                forward(x,1);
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
        float avg_loss=total_loss/cnt;
        Loss.push_back(avg_loss);
        Log<<epoch<<","<<avg_loss<<"\n";
        Log.flush();
        cout<<"Epoch "<<epoch<<", Loss: "<<avg_loss<<endl;
    }
    double elapsed_ms=(double)(clock()-start)/CLOCKS_PER_SEC*1000.0;
    Log.close();
    cout<<"训练结束"<<endl;
    cout<<"运行时间："<<elapsed_ms<<" ms"<<endl;
    plot_loss_axis(Loss);
}
int predict(const Matrix &input){
    forward(input);
    return max_element(A[L][0].begin(), A[L][0].end())-A[L][0].begin();
}
void test(){
    cout<<"测试集:"<<endl;
    vector<Matrix> X,Y;
    // load_MNIST("t10k-images.idx3-ubyte", "t10k-labels.idx1-ubyte",X,Y);
    load_optdigits("optdigits.tes",X,Y);
    int correct=0;
    float test_loss=0;
    for(int i=0;i<(int)X.size();++i){
        int predicted_label=predict(X[i]);
        if(Y[i][0][predicted_label])
            correct++;
        for(int j=0;j<layer_sizes[L];++j)
            test_loss -= Y[i][0][j] * log(A[L][0][j]+1e-12);
        // cout<<"样本 "<<i<<": True label: "<<test_labels[i]<<", Predicted label: "<<predicted_label<<endl;
    }
    cout<<"正确率: "<<correct<<"/"<<X.size()<<endl;
    cout<<"测试损失: "<<test_loss/X.size()<<endl;
}
void save_model(const string &path){
    ofstream fout(path);
    fout<<layer_sizes.size()<<"\n";
    for(int s:layer_sizes) fout<<s<<" ";
    fout<<"\n";

    for(int l=0; l<L;++l){
        auto write=[&](const Matrix &m){
            fout<<m.size()<<" "<<m[0].size()<<"\n";
            for(auto &row:m){
                for(int j=0; j<(int)row.size();++j){
                    if(j) fout<<" ";
                    fout<<row[j];
                }
                fout<<"\n";
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
        cerr<<"模型结构与当前网络不匹配!"<<endl;
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
            if(input[i][j]>0.5f){
                min_i=min(min_i, i);
                max_i=max(max_i, i);
                min_j=min(min_j, j);
                max_j=max(max_j, j);
            }
    if(max_i<0) return;  // 空图
    int ci=(min_i+max_i) / 2;
    int cj=(min_j+max_j) / 2;
    int di=h/2-ci;
    int dj=w/2-cj;
    Matrix shifted=new_matrix(h, w);
    for(int i=0;i<h;++i)
        for(int j=0;j<w;++j){
            shifted[i][j]=0.f;
            int ni=i-di, nj=j-dj;
            if(ni>=0&&ni<h&&nj>=0&&nj<w)
                shifted[i][j]=input[ni][nj];
        }
    input=shifted;
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
        cout<<"未找到模型文件，开始训练..."<<endl;
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
        }else{
            cout<<"是否测试模型？(y/n): ";
            cin>>choice;
            if(choice=='y'||choice=='Y')
                test();
        }
    }
    cout<<"按任意键继续..."<<endl;
    _getch();
    system("cls");
    cout<<"手写数字识别:"<<endl;
    cout<<"+"<<string(w, '-')<<"+\n";
    for(int i=0;i<h;++i)
        cout<<"|"<<string(w, ' ')<<"|\n";
    cout<<"+"<<string(w, '-')<<"+\n";
    Matrix input=new_matrix(h,w);
    for(int i=0;i<h;++i)
        for(int j=0;j<w;++j)
            input[i][j]=0.f;
    Setpos(1,2);
    for(int i=0;i<h;++i){
        Setpos(1,i+2);
        for(int j=0;j<w;++j)
            cout<<(input[i][j]>0.5f?"#":" ");
    }
    Setpos(0,3+h);
    cout<<"请在"<<w<<"x"<<h<<"的网格中绘制数字，'#'表示像素点，空格表示背景"<<endl;
    cout<<"wasd移动，空格绘制/擦除，回车预测，r重置，ESC退出"<<endl;
    int x=0,y=0;
    while(1){
        Setpos(x+1,y+2);
        cout<<(input[y][x]>0.5f?"#":" ");
        int ch=_getch();
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
                input[y][x]=1.f-input[y][x];
                break;
            case 114://r
                for(int i=0;i<h;++i)
                    for(int j=0;j<w;++j)
                        input[i][j]=0.f;
                for(int i=0;i<h;++i){
                    Setpos(1,i+2);
                    for(int j=0;j<w;++j)
                        cout<<(input[i][j]>0.5f?"#":" ");
                }
                break;
            case 13://enter
                {
                    Setpos(0,6+h);
                    for(int i=0;i<h;++i){
                        Setpos(0,i+6+h);
                        for(int j=0;j<w;++j)
                            cout<<(input[i][j]>0.5f?"#":" ");
                    }
                    Matrix X=new_matrix(1,layer_sizes[0]),_input=input;
                    center_digit(_input);
                    for(int i=0;i<h;++i)
                        for(int j=0;j<w;++j){
                            X[0][i*w+j]=_input[i][j]*0.6f;
                            if(i>0) X[0][i*w+j]+=_input[i-1][j]*0.1f;
                            if(i<h-1) X[0][i*w+j]+=_input[i+1][j]*0.1f;
                            if(j>0) X[0][i*w+j]+=_input[i][j-1]*0.1f;
                            if(j<w-1) X[0][i*w+j]+=_input[i][j+1]*0.1f;
                        }
                    int predicted_label=predict(X);
                    Setpos(0,5+h);
                    cout<<"预测结果: "<<predicted_label<<endl;
                }
                break;
            case 27://esc
                return 0;
        }
    }
    return 0;
}
