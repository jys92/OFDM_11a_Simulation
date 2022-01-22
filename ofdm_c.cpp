#include <iostream>
#include <vector>
#include <complex>
#include <iomanip>
using namespace std::complex_literals;

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

int main() {

    std::vector<long long int> m_hex_vec;
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
    
    std::vector<char> cc_out;
    int c_state[6] = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < m_binary_vec.size(); i++) {
        cc_out.push_back( m_binary_vec[i]^c_state[1]^c_state[2]^c_state[4]^c_state[5] );
        cc_out.push_back( m_binary_vec[i]^c_state[0]^c_state[1]^c_state[2]^c_state[5] );
        for (int j = 5; j >= 1; j--) {
            c_state[j] = c_state[j-1];
        }
        c_state[0] = m_binary_vec[i];
    }

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

    std::vector<std::complex<double>> fd_data;
    std::vector<char> mapping_in;
    
    std::vector<std::vector<char>> check_patterns = {{-1,-1,-1,-1}, {-1,-1,-1,1}, {-1,-1,1,-1},
       {-1,-1,1,1}, {-1,1,-1,-1}, {-1,1,-1,1}, {-1,1,1,-1}, {-1,1,1,1}, {1,-1,-1,-1}, 
       {1,-1,-1,1}, {1,-1,1,-1}, {1,-1,1,1}, {1,1,-1,-1}, {1,1,-1,1}, {1,1,1,-1}, {1,1,1,1}};

    /*
    for (int i = 0; i < interleaver_out.size() ; i+=N_BPSC) {
        mapping_in = {&interleaver_out[i], &interleaver_out[i+N_BPSC]};
        if      (mapping_in == check_patterns[0]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*(-3.0-3.0i);
        else if (mapping_in == check_patterns[1]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*(-3.0-1.0i);
        else if (mapping_in == check_patterns[2]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*(-3.0+3.0i);
        else if (mapping_in == check_patterns[3]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*(-3.0+1.0i);
        else if (mapping_in == check_patterns[4]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*(-1.0-3.0i);
        else if (mapping_in == check_patterns[5]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*(-1.0-1.0i);
        else if (mapping_in == check_patterns[6]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*(-1.0+3.0i);
        else if (mapping_in == check_patterns[7]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*(-1.0+1.0i);
        else if (mapping_in == check_patterns[8]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*( 3.0-3.0i);
        else if (mapping_in == check_patterns[9]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*( 3.0-1.0i);
        else if (mapping_in == check_patterns[10]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*( 3.0+3.0i);
        else if (mapping_in == check_patterns[11]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*( 3.0+1.0i);
        else if (mapping_in == check_patterns[12]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*( 1.0-3.0i);
        else if (mapping_in == check_patterns[13]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*( 1.0-1.0i);
        else if (mapping_in == check_patterns[14]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*( 1.0+3.0i);
        else if (mapping_in == check_patterns[15]) 
            fd_data[i/4] = (1.0 / std::sqrt(10.0))*( 1.0+1.0i);
    }
    */


    std::vector<char> test;
    test = {&interleaver_out[0], &interleaver_out[4]};
    char ch[4] = {-1, 1, 1, 1};
    std::vector<char> check_pattern;
    check_pattern = {-1, 1, -1, 1};
    if (test == check_pattern ) {
        std::cout << "OK" << std::endl;
    }
    if (test != check_pattern ) {
        std::cout << "NG" << std::endl;
    }


    for (auto elem : test) {
        std::cout << (int)elem << std::endl;
    }


    std::complex<double> x, y, z;
    std::vector<std::complex<double>> vec_c;


    // For pattern check!!
    std::vector<char> print_data = interleaver_out;
    int inner = 1;
    for (auto elem :print_data) {
        std::cout << std::setw(2) << (int)elem;
        if (inner%8 == 0) { 
            std::cout<<" ";
        }
        inner++;
    }
    std::cout << std::endl;

    std::cout << print_data.size() << std::endl;
    std::cout << print_data.capacity() << std::endl;
    return 0;
}



