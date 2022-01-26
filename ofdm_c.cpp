// Same with python implementation 
// IEEE802.11 17 OFDM PHY & Annex I(Can be differnt)
// For studying C++, STL, Class, template
// Final Out-put is The entire packet for the BCC example in 802.11 Annex I (Can be differnt)
// Need to implement graphic like python.
// 2022.1 final update.


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

// Need to integrate above function  
template <typename T>
void print_pattern_complex(T& print_contents) {
    int inner = 1;
    std::cout << typeid(T).name() << std::endl;
    for (auto elem : print_contents) {
        std::cout << std::setw(25) << elem;
        if (inner%4 == 0) {
            std::cout<<std::endl;
        }
        if (inner%80 == 0) {
            std::cout<<std::endl;
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

// How does template to be implemented for control both char & int
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
        std::complex<double> w = {std::cos(x), std::sin(x)};
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

template <typename T>
class Ofdm_symbol {
  private:
    T data_input_;
    static int symbol_number_;
    int pilot_index_;

  public:
    Ofdm_symbol (T data_input, int pilot_index);
    std::vector<std::complex<double>> generate();
    int symbol_count();
};

template <typename T>
int Ofdm_symbol<T>::symbol_number_ = 0;

template <typename T>
Ofdm_symbol<T>::Ofdm_symbol(T data_input, int pilot_index) {
    data_input_ = data_input;
    pilot_index_ = pilot_index;
    symbol_number_++;
}

template <typename T>
std::vector<std::complex<double>> Ofdm_symbol<T>::generate() {
    std::vector<char> pilot_sig = pilot_generator(pilot_index_);

    T fd_signal_64 = {0, 0, 0, 0, 0, 0};
    fd_signal_64.insert(fd_signal_64.end(),data_input_.begin(),data_input_.begin()+5);
    fd_signal_64.push_back(pilot_sig[0]);
    fd_signal_64.insert(fd_signal_64.end(),data_input_.begin()+5,data_input_.begin()+18);
    fd_signal_64.push_back(pilot_sig[1]);
    fd_signal_64.insert(fd_signal_64.end(),data_input_.begin()+18,data_input_.begin()+24);
    fd_signal_64.push_back(0);
    fd_signal_64.insert(fd_signal_64.end(),data_input_.begin()+24,data_input_.begin()+30);
    fd_signal_64.push_back(pilot_sig[2]);
    fd_signal_64.insert(fd_signal_64.end(),data_input_.begin()+30,data_input_.begin()+43);
    fd_signal_64.push_back(pilot_sig[3]);
    fd_signal_64.insert(fd_signal_64.end(),data_input_.begin()+43,data_input_.begin()+48);
    fd_signal_64.insert(fd_signal_64.end(),5,0);

    std::vector<std::complex<double>> fd_signal_64_c;
    fd_signal_64_c.insert(fd_signal_64_c.begin(), fd_signal_64.begin()+32, fd_signal_64.end());
    fd_signal_64_c.insert(fd_signal_64_c.end(), fd_signal_64.begin(), fd_signal_64.begin()+32);
    fft(fd_signal_64_c, true);
    std::vector<std::complex<double>> td_data_81;
    td_data_81.push_back(0.5*fd_signal_64_c[48]);
    td_data_81.insert(td_data_81.end(), fd_signal_64_c.begin()+49, fd_signal_64_c.end());
    td_data_81.insert(td_data_81.end(), fd_signal_64_c.begin(), fd_signal_64_c.end());
    td_data_81.push_back(0.5*fd_signal_64_c[0]);
    return td_data_81;    
}

template <typename T>
int Ofdm_symbol<T>::symbol_count() {
    return symbol_number_; 
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

    Ofdm_symbol<std::vector<char>> fd_signal(signal_48, 1);
    std::vector<std::complex<double>> td_signal_81 = fd_signal.generate();
//    print_pattern_complex(td_signal_81);

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

    std::vector<std::vector<char>> check_patterns = {{-1,-1,-1,-1}, {-1,-1,-1,1}, {-1,-1,1,-1},
       {-1,-1,1,1}, {-1,1,-1,-1}, {-1,1,-1,1}, {-1,1,1,-1}, {-1,1,1,1}, {1,-1,-1,-1}, 
       {1,-1,-1,1}, {1,-1,1,-1}, {1,-1,1,1}, {1,1,-1,-1}, {1,1,-1,1}, {1,1,1,-1}, {1,1,1,1}};

    std::complex<double> mapping_values[16] = {-3.0-3.0i, -3.0-1.0i, -3.0+3.0i, -3.0+1.0i, 
        -1.0-3.0i, -1.0-1.0i, -1.0+3.0i, -1.0+1.0i, 3.0-3.0i, 3.0-1.0i, 3.0+3.0i, 3.0+1.0i, 
        1.0-3.0i, 1.0-1.0i, 1.0+3.0i, 1.0+1.0i};
    for (int i = 0; i< 16; i++)
        mapping_values[i] *= (1/std::sqrt(10.0));

    std::vector<char> interleaver_block;
    std::vector<char> mapping_in;
    std::vector<std::complex<double>> fd_data_block;
    std::vector<std::complex<double>> td_data_block;
    
    std::vector<std::complex<double>> entire_symbols;

    entire_symbols.insert(entire_symbols.begin(),td_short_161.begin(), td_short_161.end()-1);
    entire_symbols.push_back(td_short_161[160]+td_long_161[0]);
    entire_symbols.insert(entire_symbols.end(),td_long_161.begin()+1, td_long_161.end()-1);
    entire_symbols.push_back(td_long_161[160]+td_signal_81[0]);
    entire_symbols.insert(entire_symbols.end(),td_signal_81.begin()+1, td_signal_81.end()-1);


    std::complex<double> last_data;
    for (int i = 0; i < Packet_Num; i++) {
        interleaver_block = interleaver_add_sign({&puncture_out[i*N_CBPS], &puncture_out[(i+1)*N_CBPS]}, N_CBPS, N_BPSC);
        for (int j = 0; j < N_CBPS; j+=N_BPSC) {
            mapping_in = {&interleaver_block[j], &interleaver_block[j+N_BPSC]};
            for (int k = 0; k < 16; k++)  // instead of 16??? 
                if (mapping_in == check_patterns[k]) 
                    fd_data_block.push_back(mapping_values[k]);
        }
        
//        print_pattern_simple(interleaver_block);
//        print_pattern_complex(fd_data_block);

        Ofdm_symbol<std::vector<std::complex<double>>> td_data(fd_data_block, i);
        td_data_block = td_data.generate();
        fd_data_block.clear();
//        print_pattern_complex(td_data_block);

        if (i == 0 ) {
            entire_symbols.push_back(td_signal_81[80]+td_data_block[0]);
            entire_symbols.insert(entire_symbols.end(), td_data_block.begin()+1, td_data_block.end()-1);    
        }
        else if (i > 0) {
            entire_symbols.push_back(last_data+td_data_block[0]);
            entire_symbols.insert(entire_symbols.end(), td_data_block.begin()+1, td_data_block.end()-1);    
        }
        last_data=td_data_block[80];        
    }

    print_pattern_complex(entire_symbols);

    return 0;
}
