#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
using namespace std;

typedef vector<vector<float>> Matrix;

mt19937 gen(random_device{}());
const int h=8,w=8;
const int n_in=w*h,n_hidden=128,n_out=10;
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
    std::vector<float> image_8x8(64, 0.0f);

    for (int by = 0; by < 8; ++by) {
        for (int bx = 0; bx < 8; ++bx) {
            int sum = 0;
            int count = 0;
            const int y_begin = by * 28 / 8;
            const int y_end = (by + 1) * 28 / 8;
            const int x_begin = bx * 28 / 8;
            const int x_end = (bx + 1) * 28 / 8;
            // 按比例划分像素区域，保证8个分区都非空且覆盖整张图。
            for (int y = y_begin; y < y_end; ++y) {
                for (int x = x_begin; x < x_end; ++x) {
                    sum += image_28x28[y * 28 + x];
                    count++;
                }
            }
            float avg = static_cast<float>(sum) / count;
            // image_8x8[by * 8 + bx] = (avg > 127.0f) ? 0.9f : -1.0f;
            image_8x8[by * 8 + bx] = avg/255.0f-0.1f;
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
    uint32_t magic = read_big_endian(img_file);
    if (magic != 2051) { std::cerr << "图像文件魔数错误!" << std::endl; return; }
    uint32_t num_images = read_big_endian(img_file);
    uint32_t rows = read_big_endian(img_file);
    uint32_t cols = read_big_endian(img_file);

    // 读取标签文件头
    uint32_t label_magic = read_big_endian(lbl_file);
    if (label_magic != 2049) { std::cerr << "标签文件魔数错误!" << std::endl; return; }
    uint32_t num_labels = read_big_endian(lbl_file);

    if (num_images != num_labels) { std::cerr << "图像与标签数量不匹配!" << std::endl; return; }

    images.clear();
    labels.clear();
    images.reserve(num_images);
    labels.reserve(num_labels);

    std::vector<uint8_t> buffer(rows * cols);

    for (uint32_t i = 0; i < num_images; ++i) {
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

void Init_images(vector<vector<float> > &train_images, vector<int> &train_labels) {
    load_mnist_8x8("train-images.idx3-ubyte", "train-labels.idx1-ubyte",
                   train_images, train_labels);
}

inline Matrix new_matrix(int n,int m){
    return Matrix(n,vector<float>(m,0.0));
}
void he_init(Matrix &a, int fan_in) {
    double std_dev = sqrt(2.0 / fan_in);
    normal_distribution<double> dist(0.0, std_dev);
    for (auto &row : a)
        for (auto &val : row)
            val = dist(gen);
}
void zero_matrix(Matrix &a, int n, int m) {
    a.assign(n, vector<float>(m, 0.0));
}
void mul(const Matrix &a,const Matrix &b,Matrix &c){
    const int n=a.size(),m=a[0].size(),p=b[0].size();
    zero_matrix(c,n,p);
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
            c[i][j] = a[i][j] > 0 ? a[i][j] : 0.01f * a[i][j];
    return c;
}

Matrix LeakyReLU_derivative(const Matrix &a){
    const int n=a.size(),m=a[0].size();
    Matrix c=new_matrix(n,m);
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j] = a[i][j] > 0 ? 1.0f : 0.01f;
    return c;
}
Matrix Sigmoid(const Matrix &a){
    const int n=a.size(),m=a[0].size();
    Matrix c=new_matrix(n,m);
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=1.0/(1.0+exp(-a[i][j]));
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
    for(int j=0;j<m;++j) {
        res[0][j]=exp(z[0][j]-max_val);
        sum+=res[0][j];
    }
    for(int j=0;j<m;++j)
        res[0][j]/=sum;
    return res;
}
Matrix augment(const Matrix &x){
    Matrix aug = x;
    // 只做随机像素翻转，不做平移
    int flips = uniform_int_distribution<int>(0, 1)(gen);
    for(int f=0; f<flips; ++f){
        int p = uniform_int_distribution<int>(0, n_in-1)(gen);
        aug[0][p] = (aug[0][p] > 0.5f) ? -0.1f : 0.9f;
    }
    return aug;
}
Matrix w1=new_matrix(n_in,n_hidden),
        w2=new_matrix(n_hidden,n_out),
        b1=new_matrix(1,n_hidden),
        b2=new_matrix(1,n_out);
void training(){
    vector<Matrix> X;
    vector<Matrix> Y;
    vector<vector<float> > train_images;
    vector<int> train_labels;
    Init_images(train_images, train_labels);
    for(const auto &img:train_images)
        X.push_back(vector<vector<float>>(1,img));
    for(const auto &label:train_labels){
        Y.push_back(vector<vector<float>>(1,vector<float>(10,0.0)));
        Y.back()[0][label]=1.0;
    }
    he_init(w1, n_in);
    he_init(w2, n_hidden);
    Matrix sum_dw1=new_matrix(n_in,n_hidden),
                sum_db1=new_matrix(1,n_hidden),
                sum_dw2=new_matrix(n_hidden,n_out),
                sum_db2=new_matrix(1,n_out);
    Matrix z1 = new_matrix(1, n_hidden);
    Matrix a1 = new_matrix(1, n_hidden);
    Matrix z2 = new_matrix(1, n_out);
    Matrix a2 = new_matrix(1, n_out);
    Matrix dz2 = new_matrix(1, n_out);
    Matrix dw2 = new_matrix(n_hidden, n_out);
    Matrix db2 = new_matrix(1, n_out);
    Matrix da1 = new_matrix(1, n_hidden);
    Matrix dz1 = new_matrix(1, n_hidden);
    Matrix dw1 = new_matrix(n_in, n_hidden);
    Matrix db1 = new_matrix(1, n_hidden);
    Matrix ta1 = new_matrix(n_hidden, 1);
    Matrix tw2 = new_matrix(n_out, n_hidden);
    Matrix tx  = new_matrix(n_in, 1);
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
            zero_matrix(sum_dw1,n_in,n_hidden);
            zero_matrix(sum_db1,1,n_hidden);
            zero_matrix(sum_dw2,n_hidden,n_out);
            zero_matrix(sum_db2,1,n_out);
            for(int k=0,pos;k<batch_size&&(pos=b*batch_size+k)<idx.size();++k){
                const Matrix &x=augment(X[idx[pos]]),&y=Y[idx[pos]];
                mul_add(x,w1,b1,z1);
                a1=LeakyReLU(z1);
                mul_add(a1,w2,b2,z2);
                a2=softmax(z2);
                float loss=0;
                for(int j=0;j<y[0].size();++j)
                    loss-=y[0][j]*log(a2[0][j]+1e-12);
                total_loss+=loss;
                sub(a2,y,dz2);
                transpose(a1, ta1);
                mul(ta1,dz2,dw2);
                db2=dz2;
                transpose(w2, tw2);
                mul(dz2,tw2,da1);
                Hadamard(da1,LeakyReLU_derivative(z1),dz1);
                transpose(x, tx);
                mul(tx,dz1,dw1);
                db1=dz1;
                add(sum_dw1,dw1,sum_dw1);
                add(sum_db1,db1,sum_db1);
                add(sum_dw2,dw2,sum_dw2);
                add(sum_db2,db2,sum_db2);
                cnt++;
            }
            sub_scalar_mul(w1,sum_dw1,lr/batch_size,w1);
            sub_scalar_mul(b1,sum_db1,lr/batch_size,b1);
            sub_scalar_mul(w2,sum_dw2,lr/batch_size,w2);
            sub_scalar_mul(b2,sum_db2,lr/batch_size,b2);
        }
        // if(epoch%30==0)
        cout<<"Epoch "<<epoch<<", Loss: "<<total_loss/cnt<<endl;
    }
    cout<<"训练结束"<<endl;
    double elapsed_ms=(double)(clock()-start)/CLOCKS_PER_SEC*1000.0;
    cout<<"运行时间："<<elapsed_ms<<" ms"<<endl;
}
/*
不开O2:570s
开O2:57s
*/
void test(){
    cout<<"测试集:"<<endl;
    vector<vector<float>> test_images;
    vector<int> test_labels;
    load_mnist_8x8("t10k-images.idx3-ubyte", "t10k-labels.idx1-ubyte",
                test_images, test_labels);
    int correct=0;
    Matrix z1=new_matrix(1,n_hidden),a1=new_matrix(1,n_hidden),
                z2=new_matrix(1,n_out),a2=new_matrix(1,n_out);
    for(int i=0;i<test_images.size();++i){
        const Matrix x(1,test_images[i]);
        mul_add(x,w1,b1,z1);
        a1=LeakyReLU(z1);
        mul_add(a1,w2,b2,z2);
        a2=softmax(z2);
        int predicted_label=max_element(a2[0].begin(),a2[0].end())-a2[0].begin();
        if(predicted_label==test_labels[i])
            correct++;
        // cout<<"样本 "<<i<<": True label: "<<test_labels[i]<<", Predicted label: "<<predicted_label<<endl;
    }
    cout<<"正确率: "<<correct<<"/"<<test_images.size()<<endl;
}
void save_model(const string& path,
                const Matrix &w1, const Matrix &b1,
                const Matrix &w2, const Matrix &b2) {
    ofstream fout(path);
    if (!fout) { cerr << "无法写入 " << path << endl; return; }

    auto write = [&](const Matrix &m){
        fout << m.size() << " " << m[0].size() << "\n";
        for (auto &row : m) {
            for (int j = 0; j < (int)row.size(); ++j) {
                if (j) fout << " ";
                fout << row[j];
            }
            fout << "\n";
        }
    };

    fout << n_in << " " << n_hidden << " " << n_out << "\n";  // 头部
    write(w1);
    write(b1);
    write(w2);
    write(b2);
    fout.close();
    cout << "模型已保存到 " << path << endl;
}
bool load_model(const string& path,
                Matrix &w1, Matrix &b1,
                Matrix &w2, Matrix &b2) {
    ifstream fin(path);
    if (!fin) { cerr << "无法打开 " << path << endl; return false; }
    int a, b, c;
    fin >> a >> b >> c;
    if(a != n_in || b != n_hidden || c != n_out){
        cerr << "模型结构与当前网络不匹配!" << endl;
        return false;
    }
    auto read = [&](Matrix &m){
        int n, cols;
        fin >> n >> cols;
        m.assign(n, vector<float>(cols));
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < cols; ++j)
                fin >> m[i][j];
    };

    read(w1);
    read(b1);
    read(w2);
    read(b2);
    fin.close();
    cout << "模型已从 " << path << " 加载" << endl;
    return true;
}
void Setpos(int x,int y){
	COORD pos;
	pos.X=x;
	pos.Y=y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
}
int main(){
    SetConsoleOutputCP(CP_UTF8);
    if(!load_model("model.txt", w1, b1, w2, b2)) {
        cout << "未找到模型文件，开始训练..." << endl;
        training();
        test();
        save_model("model.txt", w1, b1, w2, b2);
    }else{
        cout<<"是否重新训练模型？(y/n): ";
        char choice;
        cin>>choice;
        if(choice=='y'||choice=='Y'){
            cout<<"重新训练模型..."<<endl;
            training();
            test();
            save_model("model.txt", w1, b1, w2, b2);
        }
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
        int x=0,y=0,shift=0;
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
                    Matrix X=new_matrix(1,n_in);
                    for(int i=0;i<h;++i)
                        for(int j=0;j<w;++j){
                            X[0][i*w+j]=input[i][j]*0.6f;
                            if(i>0) X[0][i*w+j]+=input[i-1][j]*0.1f;
                            if(i<h-1) X[0][i*w+j]+=input[i+1][j]*0.1f;
                            if(j>0) X[0][i*w+j]+=input[i][j-1]*0.1f;
                            if(j<w-1) X[0][i*w+j]+=input[i][j+1]*0.1f;
                        }
                    Matrix z1=new_matrix(1,n_hidden),a1=new_matrix(1,n_hidden),
                            z2=new_matrix(1,n_out),a2=new_matrix(1,n_out);
                    mul_add(X,w1,b1,z1);
                    a1=LeakyReLU(z1);
                    mul_add(a1,w2,b2,z2);
                    a2=softmax(z2);
                    int predicted_label=max_element(a2[0].begin(),a2[0].end())-a2[0].begin();
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
