#include <iostream>
#include <vector>

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
        //  if( i%18!=3 && i%18!=4 && i%18!=9 && i%18!=10 && i%18!=15 && i%18!=16 ) {
            puncture_out.push_back(cc_out[i]);
        }
    }

    std::cout << std::endl;


    std::vector<char> print_data = puncture_out;
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




void BitInterleaver(int *InOut, int Length, int IDX_Stream, int N_CBPS, int N_BPSC, int PPDUFormat)
{
	int N_col, N_row, N_rot;
	int s;

	int k = 0;	// the index of the coded bit before the first permutation
	int i = 0;	// the index after the first and before the second permutation
	int j = 0;	// the index after the second permutation anf before third
	int r = 0;	// the index after the third permutation and just prior to modulation mapping
	int c = 0;

	//int *temp;
	//temp = (int *)calloc(Length, sizeof(int));
	int temp[104*6];

//	FILE *TestFile;
//	TestFile = fopen("Vector/BitIntIndex.txt", "a");	

	if(PPDUFormat == LEG20IN20)     { N_col=16; N_row=3*N_BPSC; N_rot=11; }
	else if(PPDUFormat == HT20IN20) { N_col=13; N_row=4*N_BPSC; N_rot=11; }
	else		                    { N_col=18; N_row=6*N_BPSC; N_rot=29; }
	

	s = (N_BPSC/2) ? N_BPSC/2 : 1;

	for(k=0; k<Length; k++)
	{
//		i = N_row*(k%N_col) + (int)floor(double(k/N_col));
//		j = s*(int)floor(double(i/s)) + (i+N_CBPS-(int)floor(double(N_col*i/N_CBPS)))%s;
//		c = (j-((2*IDX_Stream)%3 + 3*(int)floor(double(IDX_Stream/3)))*N_rot*N_BPSC);

		i = N_row*(k%N_col) + (k/N_col);
		j = s*(i/s) + (i+N_CBPS-((N_col*i)/N_CBPS))%s;
		c = ( j-((2*IDX_Stream) %3 + 3*(int)(IDX_Stream/3))*N_rot*N_BPSC );

//		fprintf(TestFile, "%d\t%d\t%d\t%d\n", k, i, j, c);  

		while(c<0)
			c += N_CBPS;

		r = c%N_CBPS;

		temp[r] = InOut[k];
	}

	for(i=0; i<Length; i++)
		InOut[i] = temp[i];
	
	//free(temp);
//	fclose(TestFile);

}   // BitInterleaver()

