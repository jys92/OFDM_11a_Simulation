#include <iostream>
#include <vector>
#include <complex>
#include <iomanip>
#include <typeinfo>
using namespace std::complex_literals;

template <typename T>
void print_pattern_simple(const T& print_contents) {
    int inner = 1;
    std::cout << typeid(T).name() << std::endl;
    for (auto elem : print_contents) {
        std::cout << std::setw(2) << (int)elem;
        if (inner%8 == 0) {
            std::cout<<" ";
        }
        inner++;
    }
    std::cout << std::endl;
    std::cout << " Data Size : " << print_contents.size() << std::endl;
    std::cout << " Data Capacity : " << print_contents.capacity() << std::endl;    
}

// Above function invole. 
template <typename T>
void print_pattern_complex(T& print_contents) {
    int inner = 1;
    std::cout << typeid(T).name() << std::endl;
    for (auto elem : print_contents) {
        std::cout << std::setw(8) << elem;
        if (inner%8 == 0) {
            std::cout<<" ";
        }
        inner++;
    }
    std::cout << std::endl;
    std::cout << " Data Size : " << print_contents.size() << std::endl;
    std::cout << " Data Capacity : " << print_contents.capacity() << std::endl;    
}

template<typename T>
T convolution_encode(T& cc_in) {
    T cc_out;
    int c_state[6] = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < cc_in.size(); i++) {
        cc_out.push_back( cc_in[i]^c_state[1]^c_state[2]^c_state[4]^c_state[5] );
        cc_out.push_back( cc_in[i]^c_state[0]^c_state[1]^c_state[2]^c_state[5] );
        for (int j = 5; j >= 1; j--) {
            c_state[j] = c_state[j-1];
        }
        c_state[0] = cc_in[i];
    }
    return cc_out;
}

// implement Template, control bot char & int
std::vector<char> interleaver_add_sign(std::vector<char> in_put, int n_cbps, int n_bpsc) {
    int s = std::max(n_bpsc/2, 1);
    std::vector<char> out_put(n_cbps, 0);
    int i, j;
    for (int k = 0 ; k < n_cbps ; k++) {
        i = (int)((n_cbps/16)*(k%16) + (int)(k/16));
        j = (int)(s*(int)(i/s) + (int)(i + n_cbps-(int)(16*i/n_cbps))%s);
        if(in_put[k] == 1) 
            out_put[j] = 1;
        else
            out_put[j] = -1;
    }
    return out_put;
}

std::vector<char> pilot_generator(int index) {
    int polarity_pilot[127] = { 1,1,1,1,-1,-1,-1,1,-1,-1,-1,-1, 1,1,-1,1, -1,-1,1,1, -1,1,1,-1, 
	1,1,1,1, 1,1,-1,1, 1,1,-1,1, 1,-1,-1,1, 1,1,-1,1,-1,-1,-1,1, -1,1,-1,-1, 1,-1,-1,1, 1,1,1,1, -1,-1,1,1,  
    -1,-1,1,-1, 1,-1,1,1, -1,-1,-1,1, 1,-1,-1,-1, -1,1,-1,-1, 1,-1,1,1, 1,1,-1,1, -1,1,-1,1, -1,-1,-1,-1, 
    -1,1,-1,1, 1,-1,1,-1, 1,1,1,-1, -1,1,-1,-1, -1,1,1,1, -1,-1,-1,-1, -1,-1,-1}; 
    int index_o = (index % 127) + 1;

    if (polarity_pilot[index_o] == 1)
        return {1, 1, 1, -1};
    else 
        return {-1, -1, -1, 1};    
}


void fft(std::vector<std::complex<double>>& a, bool inv) {
    int n = (int) a.size();
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        while (!((j ^= bit) & bit)) bit >>= 1;
        if (i < j) swap(a[i], a[j]);
    }
    for (int i = 1; i < n; i <<= 1) {
        double x = inv ? M_PI / i : -M_PI / i;
        std::complex<double> w = {cos(x), sin(x)};
        for (int j = 0; j < n; j += i << 1) {
            std::complex<double> th = {1, 0};
            for (int k = 0; k < i; k++) {
                std::complex<double> tmp = a[i + j + k] * th;
                a[i + j + k] = a[j + k] - tmp;
                a[j + k] += tmp;
                th *= w;
            }
        }
    }
    if (inv) {
        for (int i = 0; i < n; i++) {
            a[i] /= n;
        }
    }
}

int main() {

    std::vector<std::complex<double>> fd_short_64 = {0,0,0,0,-1.0-1i,0,0,0,-1.0-1i,0,0,0,1.0+1i,0,0,0,1.0+1i,
        0,0,0,1.0+1i,0,0,0,1.0+1i,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1.0+1i,0,0,0,-1.0-1i,0,0,0,1.0+1i,
        0,0,0,-1.0-1i,0,0,0,-1.0-1i,0,0,0,1.0+1i,0,0,0};
    for (auto& elem : fd_short_64) 
        elem *= std::sqrt(13.0/6.0);

    fft(fd_short_64, true);    

    std::vector<std::complex<double>> td_short_161;
    td_short_161.push_back(0.5*fd_short_64[32]);
    td_short_161.insert(td_short_161.end(), fd_short_64.begin()+33, fd_short_64.begin()+64);
    td_short_161.insert(td_short_161.end(), fd_short_64.begin(), fd_short_64.end());
    td_short_161.insert(td_short_161.end(), fd_short_64.begin(), fd_short_64.end());
    td_short_161.insert(td_short_161.end(), 0.5*fd_short_64[0]);    

//    print_pattern_complex(td_short_161);

    std::vector<std::complex<double>> fd_long_64 = {0,1,-1,-1,1,1,-1,1,-1,1,-1,-1,-1,-1,-1,1,1,-1,-1,1,
        -1,1,-1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1};

    fft(fd_long_64, true);

    std::vector<std::complex<double>> td_long_161;
    td_long_161.push_back(0.5*fd_long_64[32]);
    td_long_161.insert(td_long_161.end(), fd_long_64.begin()+33, fd_long_64.begin()+64);
    td_long_161.insert(td_long_161.end(), fd_long_64.begin(), fd_long_64.end());
    td_long_161.insert(td_long_161.end(), fd_long_64.begin(), fd_long_64.end());
    td_long_161.insert(td_long_161.end(), 0.5*fd_long_64[0]);
    
//    print_pattern_complex(td_long_161);

    std::vector<char> signal_24 = {1,0,1,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0};
    std::vector<char> signal_48 = convolution_encode(signal_24);

//    print_pattern_simple(signal_48);

    int N_CBPS_signal_field = 48;
    int N_BPSC_signal_field = 1;

    signal_48 = interleaver_add_sign(signal_48, N_CBPS_signal_field, N_BPSC_signal_field);
//    print_pattern_simple(signal_48);
    std::vector<char> pilot_sig = pilot_generator(1);

    std::vector<char> fd_signal_64 = {0, 0, 0, 0, 0, 0};
    
    fd_signal_64.insert(fd_signal_64.end(),signal_48.begin(),signal_48.begin()+5);
    fd_signal_64.push_back(pilot_sig[0]);
    fd_signal_64.insert(fd_signal_64.end(),signal_48.begin()+5,signal_48.begin()+18);
    fd_signal_64.push_back(pilot_sig[1]);
    fd_signal_64.insert(fd_signal_64.end(),signal_48.begin()+18,signal_48.begin()+24);
    fd_signal_64.push_back(0);
    fd_signal_64.insert(fd_signal_64.end(),signal_48.begin()+24,signal_48.begin()+30);
    fd_signal_64.push_back(pilot_sig[2]);
    fd_signal_64.insert(fd_signal_64.end(),signal_48.begin()+30,signal_48.begin()+43);
    fd_signal_64.push_back(pilot_sig[3]);
    fd_signal_64.insert(fd_signal_64.end(),signal_48.begin()+43,signal_48.begin()+48);
    fd_signal_64.insert(fd_signal_64.end(),5,0);

    std::vector<std::complex<double>> fd_signal_64_c;
    fd_signal_64_c.insert(fd_signal_64_c.begin(), fd_signal_64.begin()+32, fd_signal_64.end());
    fd_signal_64_c.insert(fd_signal_64_c.end(), fd_signal_64.begin(), fd_signal_64.begin()+32);

    fft(fd_signal_64_c, true);    
//    print_pattern_complex(fd_signal_64_c);

    std::vector<unsigned char> m_hex_vec;
    m_hex_vec ={0x04, 0x02, 0x00, 0x2E, 0x00, 0x60, 0x08, 0xCD, 0x37, 0xA6,
                0x00, 0x20, 0xD6, 0x01, 0x3C, 0xF1, 0x00, 0x60, 0x08, 0xAD,
                0x3B, 0xAF, 0x00, 0x00, 0x4A, 0x6F, 0x79, 0x2C, 0x20, 0x62,
                0x72, 0x69, 0x67, 0x68, 0x74, 0x20, 0x73, 0x70, 0x61, 0x72,
                0x6B, 0x20, 0x6F, 0x66, 0x20, 0x64, 0x69, 0x76, 0x69, 0x6E,
                0x69, 0x74, 0x79, 0x2C, 0x0A, 0x44, 0x61, 0x75, 0x67, 0x68,
                0x74, 0x65, 0x72, 0x20, 0x6F, 0x66, 0x20, 0x45, 0x6C, 0x79,
                0x73, 0x69, 0x75, 0x6D, 0x2C, 0x0A, 0x46, 0x69, 0x72, 0x65,
                0x2D, 0x69, 0x6E, 0x73, 0x69, 0x72, 0x65, 0x64, 0x20, 0x77,
                0x65, 0x20, 0x74, 0x72, 0x65, 0x61, 0x67, 0x33, 0x21, 0xB6};

    std::vector<char> m_binary_vec;
    int mask;
    for (auto elem : m_hex_vec) {
        mask = 0x01;
        for (int i = 0; i < 8; i++) {
            m_binary_vec.push_back((mask & elem) ? 1 : 0);
            mask <<= 1;
        }
    }
    m_binary_vec.insert(m_binary_vec.begin(), 16, 0);
    m_binary_vec.insert(m_binary_vec.end(), 6, 0);
    m_binary_vec.insert(m_binary_vec.end(), 42, 0);
    
    int scrambler_state[7] = {1, 0, 1, 1, 1, 0, 1};
    int middle_value;

    for (std::vector<char>::iterator iter = m_binary_vec.begin(); iter < m_binary_vec.end(); ++iter) {
        middle_value = scrambler_state[3] ^ scrambler_state[6];
        *iter = middle_value ^ (*iter);
        for (int j = 6; j > 0; j--) 
            scrambler_state[j] = scrambler_state[j-1];
        scrambler_state[0] = middle_value;
    }

    for (std::vector<char>::iterator iter = m_binary_vec.begin()+816;
            iter < m_binary_vec.begin()+822; iter++) {
        *iter = 0;
    }

    std::vector<char> cc_out = convolution_encode(m_binary_vec);

    std::vector<char> puncture_out;
    for (int i = 0; i < cc_out.size(); i++) {
        if( i%18==0||i%18==1||i%18==2||i%18==5||i%18==6||i%18==7||i%18==8||i%18==11
            ||i%18==12||i%18==13||i%18==14||i%18==17 ) {
            puncture_out.push_back(cc_out[i]);
        }
    }

    int N_CBPS = 192;
    int Byte_Data = m_hex_vec.size();
    int N_BPSC = 4;
    int Packet_Num = puncture_out.size() / N_CBPS;
    std::vector<char> interleaver_out;
    std::vector<char> temp;

    for (int i = 0; i < Packet_Num; i++) {
        temp = interleaver_add_sign({&puncture_out[i*N_CBPS], &puncture_out[(i+1)*N_CBPS]}, N_CBPS, N_BPSC);
        interleaver_out.insert(interleaver_out.end(), temp.begin(), temp.end());
    }   
//    print_pattern_simple(interleaver_out);

    std::vector<std::complex<double>> fd_data;
    std::vector<char> mapping_in;
    
    std::vector<std::vector<char>> check_patterns = {{-1,-1,-1,-1}, {-1,-1,-1,1}, {-1,-1,1,-1},
       {-1,-1,1,1}, {-1,1,-1,-1}, {-1,1,-1,1}, {-1,1,1,-1}, {-1,1,1,1}, {1,-1,-1,-1}, 
       {1,-1,-1,1}, {1,-1,1,-1}, {1,-1,1,1}, {1,1,-1,-1}, {1,1,-1,1}, {1,1,1,-1}, {1,1,1,1}};

    std::complex<double> mapping_values[16] = {-3.0-3.0i, -3.0-1.0i, -3.0+3.0i, -3.0+1.0i, 
        -1.0-3.0i, -1.0-1.0i, -1.0+3.0i, -1.0+1.0i, 3.0-3.0i, 3.0-1.0i, 3.0+3.0i, 3.0+1.0i, 
        1.0-3.0i, 1.0-1.0i, 1.0+3.0i, 1.0+1.0i};
    for (int i = 0; i< 16; i++)
        mapping_values[i] *= (1/std::sqrt(10.0));

    for (int i = 0; i < interleaver_out.size() ; i+=N_BPSC) {
        mapping_in = {&interleaver_out[i], &interleaver_out[i+N_BPSC]};
        for (int j = 0; j < 16; j++)  // instead of 16???
            if (mapping_in == check_patterns[j]) 
                fd_data.push_back(mapping_values[j]);
    }
    print_pattern_complex(fd_data);


    return 0;
}

