compile script: 
g++ fxp_check.cpp -o fxp_check -O2 -std=c++17  
glftpd.conf: pre_fxp_check	 /bin/fxp_check *  
hf
