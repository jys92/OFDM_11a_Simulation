#include <iostream>
#include <vector>

/*
Message_Hex 
Message_Hex(100) -> Message Binary(800) -> Message Binary(864)
After_Scramble_temp = Scrambler(Message 864)
After_Scramble = After_Scramble_temp[0:816]+[0 for x in range(6)] \
                 + After_Scramble_temp[822:864]
            
*/


int main() {

    std::vector<long long int> m_hex_vec;
    m_hex_vec = {0x0402002e00, 0x6008cd37a6, 0x0020d6013c, 0xf1006008ad, 0x3baf00004a, 
             0x6f792c2062, 0x7269676874, 0x2073706172, 0x6b206f6620, 0x646976696e,
             0x6974792c0a, 0x4461756768, 0x746572206f, 0x6620456c79, 0x7369756d2c,
             0x0a46697265, 0x2d696e7369, 0x7265642077, 0x6520747265, 0x61673321b6};

    std::vector<int> m_binary_vec;
    long long int mask;
    for (auto elem : m_hex_vec) {
        mask = 0x8000000000;
        for (int i = 0; i < 40; i++) {
            m_binary_vec.push_back((mask & elem) ? 1 : 0);
            mask >>= 1;
        }
    }
    m_binary_vec.insert(m_binary_vec.begin(), 16, 0);
    m_binary_vec.insert(m_binary_vec.end(), 6, 0);
    m_binary_vec.insert(m_binary_vec.end(), 42, 0);
    
    std::vector<int> after_scramble;

    int scrambler_state[7] = {1, 0, 1, 1, 1, 0, 1};
    int middle_value;
    
 

    for (std::vector<int>::iterator iter = m_binary_vec.begin(); iter < m_binary_vec.end(); iter++) {
   
        middle_value = scrambler_state[3] ^ scrambler_state[6];
        *iter = middle_value ^ (*iter);
        for (int j = 6; j > 0; j--) 
            scrambler_state[j] = scrambler_state[j-1];
        scrambler_state[0] = middle_value;
    }




    for (std::vector<int>::iterator iter = m_binary_vec.begin()+816;
            iter < m_binary_vec.begin()+822; iter++) {
        *iter = 0;
    }
    

    std::vector<char> cc_out;
    int c_stat[6] = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < m_binary_vec.size(); i++) {
        cc_out.push_back( m_binary_vec[i]^c_stat[1]^c_stat[2]^c_stat[4]^c_stat[5] );
        cc_out.push_back( m_binary_vec[i]^c_stat[0]^c_stat[1]^c_stat[2]^c_stat[5] );
        c_stat[0] = m_binary_vec[i];
        for (int j = 5; j >= 1; j--) {
            c_stat[j] = c_stat[j-1];
        }
    }

    std::vector<char> puncture_out;
    for (int i = 0; i < cc_out.size(); i++) {
//        if( i%18==0||i%18==1||i%18==2||i%18==5||i%18==6||i%18==7||i%18==8||i%18==11
//            ||i%18==12||i%18==13||i%18==14||i%18==17 ) {
        if( i%18!=3 && i%18!=4 && i%18!=9 && i%18!=10 && i%18!=15 && i%18!=16 ) {
            puncture_out.push_back(cc_out[i]);
        }
    }

    std::cout << std::endl;


    std::vector<int> print_data = m_binary_vec;
    int inner = 1;
    for (auto elem :print_data) {
        std::cout << (int)elem;
        if (inner%8 == 0) { 
            std::cout<<" ";
        }
        inner++;
    }
    std::cout << std::endl;
    std::cout << print_data.size() << std::endl;
    std::cout << print_data.capacity() << std::endl;

    std::cout << "test code====" << std::endl;
    int a = 0, b = 1;
    char c;
    c = a^b;
    std::cout << (int)c << std::endl;

    return 0;
}


