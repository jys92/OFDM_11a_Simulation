# For veiwing Time-Domain / Frequency-Domain Signal
# Originally made based on Python 2.x and checked upto 3.8.5
# Need setting Basic Level parameter for MCS
# Currently N_CBPS = 192 Byte_Data = 100 N_BPSC = 4
# Because checking for IEEE802.11 spec


import math
import numpy as np
import matplotlib.pyplot as plt

def Cnv_encd_ys(Generator_A,Generator_B,Input):
    Length = len(Input)
 
    Convolution_A = (np.convolve(Generator_A,Input)%2).tolist()
    
    Convolution_B = (np.convolve(Generator_B,Input)%2).tolist()
 
    Output = []
    for index in range(Length):
            Output += [Convolution_A[index], Convolution_B[index]]

    return Output
                             
    
def Interl(Input,n_cbps,n_bpsc):
    s = max(n_bpsc/2,1)
    Output = [0 for x in range(n_cbps)]
    for k in range(n_cbps):
            i = int((n_cbps/16)*(k%16)+math.floor(k/16))
            j = int(s*math.floor(i/s)+(i+n_cbps-math.floor(16*i/n_cbps))%s)
            Output[j] = Input[k]
            if Output[j] == 0 :
               Output[j] = -1
    return Output
            
            
def Pilot_gen(index):
    Polarity_pilot=[1,1,1,1,-1,-1,-1,1,-1,-1,-1,-1,    
                    1,1,-1,1, -1,-1,1,1, -1,1,1,-1,
		    1,1,1,1, 1,1,-1,1, 1,1,-1,1,
		    1,-1,-1,1, 1,1,-1,1,-1,-1,-1,1, 
		    -1,1,-1,-1, 1,-1,-1,1, 1,1,1,1,
		    -1,-1,1,1, -1,-1,1,-1, 1,-1,1,1,
		    -1,-1,-1,1, 1,-1,-1,-1, -1,1,-1,-1,
		    1,-1,1,1, 1,1,-1,1, -1,1,-1,1,
		    -1,-1,-1,-1, -1,1,-1,1, 1,-1,1,-1,
		    1,1,1,-1, -1,1,-1,-1, -1,1,1,1,
                    -1,-1,-1,-1, -1,-1,-1]
    index_o= (index % 127)+1
   
    return Polarity_pilot[index_o] * [1,1,1,-1]

def Scrambler (Input):
    ss = [1,0,1,1,1,0,1]
    Output = [0 for x in range(len(Input))]
    for i in range(len(Input)) :
        ss01 = (ss[3]+ss[0])%2
        Output[i] = (ss01+Input[i])%2
        ss = ss[1:7]+[ss01]

    return Output

FD_Short_64 = [0,0,0,0,-1-1j,0,0,0,-1-1j,0,0,0,1+1j,0,0,0,1+1j,0,0,0,1+1j,
               0,0,0,1+1j,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1+1j,0,0,0,-1-1j,
               0,0,0,1+1j,0,0,0,-1-1j,0,0,0,-1-1j,0,0,0,1+1j,0,0,0]
FD_Short_64 = [math.sqrt(13.0/6.0)*x for x in FD_Short_64]
IFFT_Short_64 = np.array(np.fft.ifft(FD_Short_64)).tolist()
TD_Short_161 = [0.5*IFFT_Short_64[32]]+IFFT_Short_64[33:64]+IFFT_Short_64 \
               + IFFT_Short_64+[0.5*IFFT_Short_64[0]]

FD_Long_64 = [0,1,-1,-1,1,1,-1,1,-1,1,-1,-1,-1,-1,-1,1,1,-1,-1,1,-1,1,-1,1,
              1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,-1,-1,1,1,-1,1,-1,1,1,1,1,1,
              1,-1,-1,1,1,-1,1,-1,1,1,1,1]
IFFT_Long_64 = np.array(np.fft.ifft(FD_Long_64)).tolist()
TD_Long_161 = [0.5*IFFT_Long_64[32]]+IFFT_Long_64[33:64]+IFFT_Long_64 \
              +IFFT_Long_64+[0.5*IFFT_Long_64[0]]

Conv_Gen_Mat_A = [1,0,1,1,0,1,1]
Conv_Gen_Mat_B = [1,1,1,1,0,0,1]
Signal_24 = [1,0,1,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0]
              
Signal_48_cnv = Cnv_encd_ys(Conv_Gen_Mat_A,Conv_Gen_Mat_B,Signal_24)

Ncbps = 48
Nbpsc = 1
Signal_48 = Interl(Signal_48_cnv,Ncbps,Nbpsc)
Pilot=Pilot_gen(1)

FD_Signal_64 = [0,0,0,0,0,0]+Signal_48[0:5]+[Pilot[0]]+Signal_48[5:18] \
               +[Pilot[1]]+Signal_48[18:24]+[0]+Signal_48[24:30]+[Pilot[2]] \
               +Signal_48[30:43]+[Pilot[3]]+Signal_48[43:48]+[0,0,0,0,0]

FD_Signal_64 = FD_Signal_64[32:64]+FD_Signal_64[0:32]

IFFT_Signal_64 = np.array(np.fft.ifft(FD_Signal_64)).tolist()
TD_Signal_81 = [0.5*IFFT_Signal_64[48]]+IFFT_Signal_64[49:64] \
               + IFFT_Signal_64[0:64]+[0.5*IFFT_Signal_64[0]]

N_CBPS = 192
Byte_Data = 100
N_BPSC = 4

Message_Hex ='0402002E00'+'6008cd37A6'+'0020d6013c'+'f1006008ad'+'3baf00004a' \
              +'6f792c2062'+'7269676874'+'2073706172'+'6b206f6620'+'646976696e' \
              +'6974792c0a'+'4461756768'+'746572206f'+'6620456c79'+'7369756d2c' \
              +'0a46697265'+'2d696e7369'+'7265642077'+'6520747265'+'61da5799ed'


Message_Bin_800 = ''
for i in range(0,len(Message_Hex),2) :
    Message_Bin_800 = Message_Bin_800+(bin(int(Message_Hex[i:i+2],16))[2:]).zfill(8)[::-1]
    
Message_Bin_800 = [int(x) for x in Message_Bin_800]
Message_Bin_864 = [0 for x in range(16)]+Message_Bin_800+[0 for x in range(6)] \
                  +[0 for x in range(42)]

After_Scramble_temp = Scrambler(Message_Bin_864)
After_Scramble = After_Scramble_temp[0:816]+[0 for x in range(6)] \
                 + After_Scramble_temp[822:864]

After_Coding_1_2 = Cnv_encd_ys(Conv_Gen_Mat_A,Conv_Gen_Mat_B,After_Scramble)
After_Coding_3_4 = [0 for x in range(1152)]
for i in range(1152) :
    After_Coding_3_4[i] = After_Coding_1_2[i+int(2*math.floor((i+1)/4))]

After_Interl = [0 for x in range(len(After_Coding_3_4))]
Packet_Num = 1152//N_CBPS

for i in range(Packet_Num) :
    After_Interl[(i*N_CBPS):((i+1)*N_CBPS)] \
    = Interl(After_Coding_3_4[(i*N_CBPS):((i+1)*N_CBPS)],N_CBPS,N_BPSC)

FD_Data = [0 for x in range(len(After_Interl)//4)]    
for i in range(0,1152,4) :
    if After_Interl[i:i+4] == [-1,-1,-1,-1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(-3-3j)
    elif After_Interl[i:i+4] == [-1,-1,-1,1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(-3-1j)
    elif After_Interl[i:i+4] == [-1,-1,1,-1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(-3+3j)
    elif After_Interl[i:i+4] == [-1,-1,1,1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(-3+1j)
    elif After_Interl[i:i+4] == [-1,1,-1,-1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(-1-3j)
    elif After_Interl[i:i+4] == [-1,1,-1,1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(-1-1j)
    elif After_Interl[i:i+4] == [-1,1,1,-1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(-1+3j)
    elif After_Interl[i:i+4] == [-1,1,1,1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(-1+1j)
    elif After_Interl[i:i+4] == [1,-1,-1,-1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(3-3j)
    elif After_Interl[i:i+4] == [1,-1,-1,1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(3-1j)    
    elif After_Interl[i:i+4] == [1,-1,1,-1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(3+3j)
    elif After_Interl[i:i+4] == [1,-1,1,1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(3+1j)    
    elif After_Interl[i:i+4] == [1,1,-1,-1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(1-3j)       
    elif After_Interl[i:i+4] == [1,1,-1,1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(1-1j)
    elif After_Interl[i:i+4] == [1,1,1,-1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(1+3j)    
    elif After_Interl[i:i+4] == [1,1,1,1] :
        FD_Data[i//4] = (1/math.sqrt(10))*(1+1j)

Pilot_Data = [[1,1,1,-1],[1,1,1,-1],[1,1,1,-1],[-1,-1,-1,1],[-1,-1,-1,1],[-1,-1,-1,1]]
        
FD_Data_64 = [0 for x in range(64)]
IFFT_Data_64 = [0 for x in range(64)]
TD_Data_81 = [0 for x in range(80*Packet_Num+1)]


for i in range(Packet_Num) :
    FD_Data_64 = [0 for x in range(6)]+FD_Data[(48*i):(48*i+5)]+[Pilot_Data[i][0]] \
                 +FD_Data[(48*i+5):(48*i+18)]+[Pilot_Data[i][1]]+FD_Data[(48*i+18):(48*i+24)] \
                 +[0]+FD_Data[(48*i+24):(48*i+30)]+[Pilot_Data[i][2]]+FD_Data[(48*i+30):(48*i+43)] \
                 +[Pilot_Data[i][3]]+FD_Data[(48*i+43):(48*i+48)]+[0 for x in range(5)]
    FD_Data_64 = FD_Data_64[32:64]+FD_Data_64[0:32]
    IFFT_Data_64 = np.array(np.fft.ifft(FD_Data_64)).tolist()
    if i == 0 :
        TD_Data_81 = [IFFT_Data_64[48]*0.5]+IFFT_Data_64[49:64]+IFFT_Data_64[0:64]+[IFFT_Data_64[0]*0.5]
    else :
        TD_Data_81 = TD_Data_81[0:80*i]+[TD_Data_81[80*i]+0.5*IFFT_Data_64[48]]+IFFT_Data_64[49:64]\
                     +IFFT_Data_64[0:64]+[IFFT_Data_64[0]*0.5]

Entire_Packet = TD_Short_161[0:160]+[TD_Short_161[160]+TD_Long_161[0]]+TD_Long_161[1:160]\
                +[TD_Long_161[160]+TD_Signal_81[0]]+TD_Signal_81[1:80]+[TD_Signal_81[80]+TD_Data_81[0]]\
                +TD_Data_81[1:]


# Graphic Process #
plt.subplot(3,2,1)
plt.plot(range(len(TD_Short_161)),[abs(TD_Short_161[x]) for x in range(len(TD_Short_161))])

plt.subplot(3,2,2)
FFT_TD_Short_161 = 10*np.log10(abs(np.fft.fftshift(np.fft.fft(TD_Short_161))))
plt.plot(range(len(FFT_TD_Short_161)),[FFT_TD_Short_161[x] for x in range(len(FFT_TD_Short_161))])
plt.ylim([-10,10])

plt.subplot(3,2,3)
plt.plot(range(len(TD_Long_161)),[abs(TD_Long_161[x]) for x in range(len(TD_Long_161))])

plt.subplot(3,2,4)
FFT_TD_Long_161 = 10*np.log10(abs(np.fft.fftshift(np.fft.fft(TD_Long_161))))
plt.plot(range(len(FFT_TD_Long_161)),[FFT_TD_Long_161[x] for x in range(len(FFT_TD_Long_161))])
plt.ylim([-10,10])

plt.subplot(3,2,5)
plt.plot(range(len(Entire_Packet)),[abs(Entire_Packet[x]) for x in range(len(Entire_Packet))])

plt.subplot(3,2,6)
FFT_Entire_Packet = 10*np.log10(abs(np.fft.fftshift(np.fft.fft(Entire_Packet))))
# FFT_Entire_Packet=np.array(FFT_Entire_Packet).tolist()  # Not necessary
plt.plot(range(len(FFT_Entire_Packet)),[FFT_Entire_Packet[x] for x in range(len(FFT_Entire_Packet))])
plt.ylim([-10,10])

plt.ioff()    
plt.show()
