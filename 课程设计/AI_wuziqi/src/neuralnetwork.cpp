#include "neuralnetwork.h"

neural_matrix new_neural_matrix(int n,int m){
    return neural_matrix(n,vector<float>(m,0.0));
}

void he_init(neural_matrix &a, int fan_in) {
    double std_dev=sqrt(2.0/fan_in);
    normal_distribution<double> dist(0.0, std_dev);
    for (auto &row:a)
        for (auto &val:row)
            val=dist(rng);
}
void zero_inplace(neural_matrix &a){
    for(auto &row : a)
        fill(row.begin(), row.end(), 0.0f);
}
void mul(const neural_matrix &a,const neural_matrix &b,neural_matrix &c){
    const int n=a.size(),m=a[0].size(),p=b[0].size();
    zero_inplace(c);
    for(int i=0;i<n;++i)
        for(int k=0;k<m;++k){
            float aik=a[i][k];
            for(int j=0;j<p;++j)
                c[i][j]+=aik*b[k][j];
        }
}
void add(const neural_matrix &a,const neural_matrix &b,neural_matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]+b[i][j];
}
void mul_add(const neural_matrix &a,const neural_matrix &b,const neural_matrix &c,neural_matrix &d){
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
void sub(const neural_matrix &a,const neural_matrix &b,neural_matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]-b[i][j];
}
void scalar_mul(const neural_matrix &a,float b,neural_matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]*b;
}
void sub_scalar_mul(const neural_matrix &a,const neural_matrix &b,float c,neural_matrix &d){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            d[i][j]=a[i][j]-b[i][j]*c;
}
void transpose(const neural_matrix &a, neural_matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[j][i]=a[i][j];
}
void LeakyReLU(const neural_matrix &a,neural_matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]>0?a[i][j]:0.01f*a[i][j];
}
void LeakyReLU_hadamard(const neural_matrix &a,const neural_matrix &b,neural_matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=(a[i][j]>0?a[i][j]:0.01f*a[i][j])*b[i][j];
}
neural_matrix LeakyReLU_derivative(const neural_matrix &a){
    const int n=a.size(),m=a[0].size();
    neural_matrix c=new_neural_matrix(n,m);
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]>0?1.0f:0.01f;
    return c;
}
void Hadamard(const neural_matrix &a,const neural_matrix &b,neural_matrix &c){
    const int n=a.size(),m=a[0].size();
    for(int i=0;i<n;++i)
        for(int j=0;j<m;++j)
            c[i][j]=a[i][j]*b[i][j];
}
neural_matrix softmax(const neural_matrix &z) {
    const int m=z[0].size();
    float max_val=z[0][0];
    for(int j=1;j<m;++j)
        max_val=max(max_val,z[0][j]);
    float sum=0;
    neural_matrix res=new_neural_matrix(1, m);
    for(int j=0;j<m;++j){
        res[0][j]=exp(z[0][j]-max_val);
        sum+=res[0][j];
    }
    for(int j=0;j<m;++j)
        res[0][j]/=sum;
    return res;
}
void neural_network::init_network()const{
    W.resize(L);
    B.resize(L);
    for(int l=0;l<L;++l){
        W[l]=new_neural_matrix(layer_sizes[l],layer_sizes[l+1]);
        B[l]=new_neural_matrix(1,layer_sizes[l+1]);
        he_init(W[l],layer_sizes[l]);
        // B 保持 0
    }
}
void neural_network::init_buffers()const{
    for(int l=0;l<L;++l){
        Z[l]=new_neural_matrix(1,layer_sizes[l+1]);
        dZ[l]=new_neural_matrix(1,layer_sizes[l+1]);
        dW[l]=new_neural_matrix(layer_sizes[l],layer_sizes[l+1]);
        sum_dW[l]=new_neural_matrix(layer_sizes[l],layer_sizes[l+1]);
        sum_dB[l]=new_neural_matrix(1,layer_sizes[l+1]);
        tA[l]=new_neural_matrix(layer_sizes[l],1);
        tW[l]=new_neural_matrix(layer_sizes[l+1],layer_sizes[l]);
        dA[l]=new_neural_matrix(1,layer_sizes[l]);
        vW[l]=new_neural_matrix(layer_sizes[l],layer_sizes[l+1]);
        vB[l]=new_neural_matrix(1,layer_sizes[l+1]);
        D[l]=new_neural_matrix(1,layer_sizes[l+1]);
    }
    for(int l=0;l<=L;++l)
        A[l]=new_neural_matrix(1,layer_sizes[l]);
}
void neural_network::forward(const neural_matrix &x,bool mode=0)const{
    A[0]=x;
    uniform_real_distribution<float> dist(0,1);
    for(int l=0;l<L;++l){
        mul_add(A[l],W[l],B[l],Z[l]);
        if(l<L-1){
            if(mode){
                for(int i=0;i<layer_sizes[l+1];++i)
                    D[l][0][i]=(dist(rng)>p)/(1-p);
                LeakyReLU_hadamard(Z[l],D[l],A[l+1]);
            }else LeakyReLU(Z[l],A[l+1]);
        }else A[l+1]=softmax(Z[l]);
    }
}
void neural_network::backward(const neural_matrix &y)const{
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
void neural_network::update_params(float scale)const{
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
int neural_network::predict(const neural_matrix &input)const{
    forward(input);
    return max_element(A[L][0].begin(), A[L][0].end())-A[L][0].begin();
}
void neural_network::load_data(string filename,vector<neural_matrix> &X,vector<neural_matrix> &Y)const{
    ifstream file(filename);
    if (!file.is_open()){
        cerr<<"无法打开文件: "<<filename<<endl;
        return;
    }
    try{
        string line;
        X.clear();
        Y.clear();
        while(getline(file, line)) {
            stringstream ss(line);
            string value;
            
        }
        file.close();
    }catch(...){
        cerr<<"fail to load model"<<endl;
        exit(-1);
    }
    cout<<"加载完成: "<<X.size()<<" 个样本"<<endl;
}
void neural_network::training()const{
    vector<neural_matrix> X,Y;
    load_data("data.txt",X,Y);
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
        shuffle(idx.begin(),idx.end(),rng);
        for(int b=0;b<num_batches;++b){
            for(int l=0;l<L;++l){
                zero_inplace(sum_dW[l]);
                zero_inplace(sum_dB[l]);
            }
            for(int k=0,pos;k<batch_size&&(pos=b*batch_size+k)<(int)idx.size();++k){
                const neural_matrix &x=X[idx[pos]],&y=Y[idx[pos]];
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
}
void neural_network::save_model(const string &path)const{
    ofstream fout(path);
    fout<<layer_sizes.size()<<"\n";
    for(int s:layer_sizes) fout<<s<<" ";
    fout<<"\n";

    for(int l=0; l<L;++l){
        auto write=[&](const neural_matrix &m){
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
bool neural_network::load_model(const string &path)const{
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
        auto read=[&](neural_matrix &m){
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
Action neural_network::computeMove(vector<vector<int>> mp)const{

}