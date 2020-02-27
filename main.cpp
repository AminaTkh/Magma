#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <string>
using namespace std;


vector <uint8_t> keylong(32);
int filesize, u, argc_new;
int flag_op = -1;
char* argv_new[100];
const char * iv_filename = NULL;
const char * input_filename = NULL;
const char * output_filename = NULL;
const char * key_filename = NULL;
int iv_open_flag = 0;
string mode_name;



int get_file_size(const char * filename) 
{
    FILE *p_file = NULL;
    if (filename == NULL) {   
      p_file = freopen (filename, "rb", stdin); 
      int i = 0;
      vector <uint8_t> a(1) ;
      FILE * outputf = fopen ("helpme.bin", "r+b");
   while (fread(&a[0], sizeof(uint8_t ), 1, p_file)){
        i++;
        fwrite (&a[0], sizeof (uint8_t), 1, outputf);
      }
    return i;
    }
    else
    p_file = fopen(filename ,"rb");
    fseek(p_file,0,SEEK_END);
    int size = ftell(p_file);
    fclose(p_file);
    return size; 
}



void read_file ( vector<uint8_t> &b, int size, int poz = 0){
  FILE * input;
  if (input_filename == NULL) input = fopen("helpme.bin", "rb"); else
  input = fopen (input_filename, "rb"); 
  fseek(input, poz, SEEK_SET); 
  for (int i = 0; i < size; i++){
    fread (&b[i], sizeof (uint8_t), 1, input);
  //  cout << hex <<(int)b[i];
  }
  fclose(input);
}



void write_file ( vector<uint8_t> &b, int size, int poz = 0){
  if (output_filename == NULL) {
    for (int i = 0; i < size; i++)
      cout << hex << (int)b[i];
      return;
  }
FILE * output = fopen (output_filename, "r+b");
if (output == NULL){
  perror ("error with opening file for writing");
  exit (0);
}
    poz = get_file_size(output_filename);
    fseek(output, poz, SEEK_SET);
for (int i = 0; i < size; i++)
  size_t a = fwrite (&b[i], sizeof (uint8_t), 1, output);
fclose(output);
}




void read_file_key(const char*filename,vector <uint8_t> &a){
FILE * input = fopen (filename, "r");
if (input == NULL){
  perror ("error with opening key file");
  exit (0);
}
  size_t rez;
 	int i = 0;
  while (fread (&a[i], sizeof (uint8_t), 1, input)){
		i++;
	}
	if (i != 32){	
		perror("the size of key file is wrong");
		exit(0);
}
}



void read_file_iv_for_ctr(vector <uint8_t> &a){
  if (iv_filename == NULL){
    for (int j = 0; j <8; j++)
      a[j] = 0;
  } else {
    FILE * input = fopen (iv_filename, "r");
    if (input == NULL){
        perror ("error with opening iv file");
        exit (0);
    }
    int i = 0;
    size_t rez;
    while (fread (&a[i], sizeof (uint8_t), 1, input)){
      i++;
    }
    if (i != 4){ 
        perror("the size of iv file for ctr is wrong");
        exit(0);
    }
    for (i=5; i<8; i++) 
        a[i] = 0;  
    }
  iv_open_flag = 1;
}


void read_file_iv_for_ofb(vector <uint8_t> &a, int poz = 0){
   if (iv_filename == NULL){
        iv_filename = "iv.bin";
        FILE * inputv = fopen(iv_filename, "r+b");
        for (int j = 0; j < 8; j++)
            a[j] = 0;
        for (int j = 0; j < 8; j++)
           fwrite(&a[j], sizeof(uint8_t), 1, inputv); 
  } else {
    FILE * inputv = fopen(iv_filename, "r+b");
    if (inputv == NULL){
      perror ("error with opening file");
      exit (0);
    }
    if (poz == -1){
        poz = get_file_size(iv_filename);
        fseek(inputv, poz, SEEK_SET);
        for (int i = 0; i < 8; i++)
        fwrite(&a[i], sizeof(uint8_t), 1, inputv); 
    } else {
        fseek(inputv, poz, SEEK_SET);
        int i = 0;
        while (fread (&a[i], sizeof (uint8_t), 1, inputv)){
           i++;
        }
       }
         fclose(inputv);
     }
}






void T(vector <uint8_t> word, vector <uint8_t> &rez)
{
	unsigned char Pi[8][16]=
{
  {1,7,14,13,0,5,8,3,4,15,10,6,9,12,11,2},
  {8,14,2,5,6,9,1,12,15,4,11,0,13,10,3,7},
  {5,13,15,6,9,2,12,10,11,7,8,1,4,3,14,0},
  {7,15,5,10,8,1,6,13,0,9,3,14,11,4,2,12},
  {12,8,2,1,13,4,15,6,7,0,10,5,3,14,9,11},
  {11,3,5,8,2,15,10,13,14,1,7,4,12,9,6,0},
  {6,8,2,3,9,10,5,12,1,14,4,7,11,13,0,15},
  {12,4,6,2,10,5,11,9,14,8,13,7,0,3,15,1}
};
  uint8_t first, second;
  int i;
  for (i = 0; i < 4; i++)
  {
    first = (word[i] & 0xf0) >> 4;
    second = (word[i] & 0x0f);
    first = Pi[i * 2][first];
    second = Pi[i * 2 + 1][second];
    rez[i] = (first << 4) | second;
  }
} 


 vector <vector<uint8_t> > iter_key(32, vector<uint8_t> (4,0));


void Expand_Key(vector <uint8_t> key)
{ 
	for (int i = 0; i <=7; i++){
  		iter_key[i][0] = key[i*4];
  		iter_key[i][1] = key[i*4+1];
  		iter_key[i][2] = key[i*4+2];
  		iter_key[i][3] = key[i*4+3];
  		iter_key[i+8][0] = key[i*4];
  		iter_key[i+8][1] = key[i*4+1];
  		iter_key[i+8][2] = key[i*4+2];
  		iter_key[i+8][3] = key[i*4+3];
  		iter_key[i+16][0] = key[i*4];
  		iter_key[i+16][1] = key[i*4+1];
  		iter_key[i+16][2] = key[i*4+2];
  		iter_key[i+16][3] = key[i*4+3];
 	}
 for (int i = 0; i <=7; i++){
  		iter_key[i+24][0]=key[28 - i*4];
  		iter_key[i+24][1]=key[28 - i*4+1];
  		iter_key[i+24][2]=key[28 - i*4+2];
  		iter_key[i+24][3]=key[28 - i*4+3];
  	}
}


void Add_mod(vector <uint8_t> a, vector <uint8_t> b, vector <uint8_t> &c)
{
  int i;
  unsigned int t = 0;
  for (i = 3; i >= 0; i--)
  {
    t = a[i] + b[i] + (t >> 8);
    c[i] = t & 0xff;
  }
}



void Add(vector  <uint8_t> a, vector <uint8_t> b, vector <uint8_t> &c)
{
  int i;
  for (i = 0; i < 4; i++)
    c[i] = a[i]^b[i];
}


void Add_mod_8(vector <uint8_t> a, vector <uint8_t> b, vector <uint8_t> &c)
{
  int i;
  uint8_t ost = 0;
  for (i = 7; i >= 0; i--){
    c[i] = a[i]+b[i]+ost;
    if ((int)a[i] + (int)b[i] > 255) 
      ost = 1; else 
    ost = 0;
  }
}



void Add_8(vector  <uint8_t> a, vector <uint8_t> b, vector <uint8_t> &c)
{
  int i;
  for (i = 0; i < 8; i++)
    c[i] = a[i]^b[i];
}



void g(vector <uint8_t> k, vector <uint8_t> a, vector <uint8_t> &rez)
{
  vector <uint8_t>t(4);
  uint32_t rez_32;
  Add_mod(a, k, t);
  T(t, t);
  rez_32 = t[0];
  rez_32 = (rez_32 << 8) + t[1];
  rez_32 = (rez_32 << 8) + t[2];
  rez_32 = (rez_32 << 8) + t[3];
  rez_32 = (rez_32 << 11)|(rez_32 >> 21);
  rez[3] = rez_32;
  rez[2] = rez_32 >> 8;
  rez[1] = rez_32 >> 16;
  rez[0] = rez_32 >> 24;
}



void G_(vector <uint8_t> k, vector <uint8_t> a, vector <uint8_t> &rez)
{
  vector <uint8_t> a0(4); 
  vector <uint8_t> a1(4); 
  vector <uint8_t> G(4);
  int i;
  for(i = 0; i < 4; i++)
  {
    a0[i] = a[4 + i];
    a1[i] = a[i];
  }  
  g(k, a0, G);
  Add(a1, G, G);
  for(i = 0; i < 4; i++)
  {
    a1[i] = a0[i];
    a0[i] = G[i];
  }
  for(i = 0; i < 4; i++)
  {
    rez[i] = a1[i];
    rez[4 + i] = a0[i];
  }
}





void G_Fin( vector <uint8_t> k, vector <uint8_t> a, vector <uint8_t> &rez)
{
  vector <uint8_t> a0(4); 
  vector <uint8_t> a1(4); 
  vector <uint8_t> G(4);
  int i;
  for(i = 0; i < 4; i++)
  {
    a0[i] = a[4 + i];
    a1[i] = a[i];
  }
   g(k, a0, G);
  Add(a1, G, G);
  for(i = 0; i < 4; i++)
    a1[i] = G[i];
   for(i = 0; i < 4; i++)
  {
    rez[i] = a1[i];
    rez[4 + i] = a0[i];
  }
}



void Encrypt(vector <uint8_t> word, vector <uint8_t> &rez)
{
  int i;
  G_(iter_key[0], word, rez);
  for(i = 1; i < 31; i++)
   G_(iter_key[i], rez, rez);
  G_Fin(iter_key[31], rez, rez);
}



void Decrypt(vector <uint8_t> blk, vector <uint8_t> &out_blk)
{
  int i;
  G_(iter_key[31], blk, out_blk);
  for (i = 30; i > 0; i--)
    G_(iter_key[i], out_blk, out_blk);
  G_Fin(iter_key[0], out_blk, out_blk);
}



void copy_to_mas (vector <uint8_t> &to, vector <uint8_t> from, int sizefrom, int sizeto){
  for (int i = 0; i<8; i++)
    to[sizeto+i] = from[sizefrom+i];  
}









void Proc_2(vector <uint8_t> &word, int size, int how_we_need_more){
    word[size] = 0x80;
    int i;
    for (i = size + 1; i < size + how_we_need_more; i++) 
      word[i] = 0x00;
}


void ECB_Encrypt_File(vector <uint8_t> key, int size)
{
    vector <uint8_t> word(8), rez(8);
    int kol = 0;
    Expand_Key(key);
    while (size)
    {
        if (size > 8)
        {
            read_file(word, 8, 8*kol);
            Encrypt(word, rez);
            write_file(rez, 8, 8*kol);
            size = size - 8;
        }
        else
        {
           read_file(word, size, 8*kol);
           Proc_2(word, size, 8-(size%8));
           Encrypt(word, rez);
           write_file(rez, 8, 8*kol);
           size = 0;
        }
        kol++;
    }
}


void ECB_Decrypt_File(vector <uint8_t> key, int size)
{
    vector <uint8_t> word(8), rez(8);
    int kol = 0;
    Expand_Key(key);
    while (size)
    {
        if (size > 8)
        {
            read_file(word, 8, 8*kol);
            Decrypt(word, rez);
            write_file(rez, 8, 8*kol);            
            size = size - 8;
        }
        else
        {
           read_file(word, size, 8*kol);
           Proc_2(word, size, 8-(size%8));
           Decrypt(word, rez);
           write_file(rez, size, 8*kol);           
           size = 0;
        }
        kol++;
    }
}






vector <uint8_t> Make_CTR(vector <uint8_t> mas)
{
  vector <uint8_t> b(8), newctr(8);
  for (int i = 0; i<7; i++)
    b[i] = 0;
    b[7] = 1;
    if (iv_open_flag  == 0)
      read_file_iv_for_ctr(newctr);
       else 
        Add_mod_8(mas, b, newctr);
  return newctr;
}


void CTR_File(vector <uint8_t> key, int size)
{
    Expand_Key(key);
    int kol = 0;
    vector <uint8_t> word(8), rez(8), ctr(8), enc_ctr(8);
    while (size)
    {
        if (size >= 8)
        {
           read_file( word, 8, kol * 8 );
           ctr = Make_CTR(ctr);
           Encrypt(ctr,enc_ctr);
           Add_8(word, enc_ctr, rez);
           write_file( rez, 8, 8*kol);           
           size = size - 8;
        }
        else
        {
           read_file(word, size, kol * 8);
           ctr = Make_CTR(ctr);
           Encrypt(ctr,enc_ctr);
           int i;
           for (i = 0; i < size; i++)
           rez[i] = word[i]^enc_ctr[i];
           write_file(rez, size, 8*kol);
           size = 0;
        }
        kol ++;
    }
}




void OFB_File( vector <uint8_t> key, int size){
    Expand_Key(key);
    int kol = 0;
    vector <uint8_t> r(8), rez(8), word(8), enc_r(8);
    while (size)
    {
        if (size > 8)
        {
           read_file( word, 8, kol * 8);
           read_file_iv_for_ofb( r, kol * 8);
           Encrypt(r,r);
           read_file_iv_for_ofb( r, -1);
           Add_8(word, r, rez);
           write_file( rez, 8, 8*kol);
           size = size-8;
        }
        else
        {
           read_file( word, size, kol * 8);
           read_file_iv_for_ofb(r, kol * 8);
           Encrypt(r,r);
           for (int i = 0; i < size; i++)
            rez[i] = word[i]^r[i];
           write_file(rez, size, 8*kol);
           size = 0;
        }
        kol ++;
    }
}




void CBC_File_Encrypt( vector <uint8_t> key, int size){
    Expand_Key(key);
    int kol = 0;
    vector <uint8_t> r(8), rez(8), word(8), enc_r(8);
    while (size)
    {
        if (size > 8)
        {
           read_file(word, 8, kol * 8);
           read_file_iv_for_ofb( r, kol * 8);
           Add_8(word, r, rez);
           Encrypt(rez,rez);
           read_file_iv_for_ofb( rez, -1);
           write_file(rez, 8, 8*kol);
           size = size-8;
        }
        else
        {
           read_file( word, size, kol * 8);
           Proc_2(word, size, 8-(size%8));
           read_file_iv_for_ofb( r, kol * 8);
           for (int i = 0; i < size; i++)
            rez[i] = word[i]^r[i];
           Encrypt(rez,rez);
           write_file( rez, size, 8*kol);
           size = 0;
        }
        kol ++;
    }
}


void CBC_File_Decrypt(vector <uint8_t> key, int size){
    Expand_Key(key);
    int kol = 0;
    vector <uint8_t> r(8), rez(8), word(8), enc_r(8);
    while (size)
    {
        if (size > 8)
        {
           read_file( word, 8, kol * 8);
           read_file_iv_for_ofb( word, -1);
           read_file_iv_for_ofb( r, kol * 8);
           Decrypt(word, word);
           Add_8(word, r, rez);
           write_file( rez, 8, 8*kol);
           size = size-8;
        }
        else
        {
           read_file( word, size, kol * 8);
           read_file_iv_for_ofb( r, kol * 8);
           Decrypt(word, word);
           Add_8(word, r, rez);          
           write_file( rez, 8, 8*kol);
           size = 0;
        }
        kol ++;
    }
}




void CFB_File_Encrypt(vector <uint8_t> key, int size){
    Expand_Key(key);
    int kol = 0; 
    vector <uint8_t> r(8), rez(8), word(8), enc_r(8);
    while (size)
    {
        if (size > 8)
        {
           read_file( word, 8, kol * 8);
           read_file_iv_for_ofb(r, kol * 8);
           Encrypt(r,r);
           Add_8(word, r, rez);
           read_file_iv_for_ofb( rez, -1); 
           write_file( rez, 8, 8*kol);
           size = size-8;
        }
        else
        {
           read_file(word, size, kol * 8);
           Proc_2(word, size, 8-(size%8));
           read_file_iv_for_ofb( r, kol * 8);
           Encrypt(r,r);
           for (int i = 0; i < 8; i++)
            rez[i] = word[i]^r[i];
           write_file( rez, 8, 8*kol);
           size = 0;
        }
        kol ++;
    }
}


void CFB_File_Decrypt(vector <uint8_t> key, int size){
    Expand_Key(key);
    int kol = 0; 
    vector <uint8_t> r(8), rez(8), word(8), enc_r(8);
    while (size)
    {
        if (size > 8)
        {
           read_file( word, 8, kol * 8);
           read_file_iv_for_ofb(r, kol * 8);
           Encrypt(r,r);
           Add_8(word, r, rez);
           read_file_iv_for_ofb( word, -1);
           write_file( rez, 8, 8*kol);
           size = size-8;
        }
        else
        {
           read_file( word, size, kol * 8);
           read_file_iv_for_ofb( r, kol * 8);
           Encrypt(r,r);
           for (int i = 0; i < 8; i++)
            rez[i] = word[i]^r[i];
           write_file( rez, 8, 8*kol);
           size = 0;
        }
        kol ++;
    }
}



void mult2(vector <uint8_t> a, vector <uint8_t> &k){
    unsigned int t = 0;
  for (int i = 7; i >= 0; i--)
  {
    t = a[i] * 0x02 + (t >> 8);
    k[i] = t & 0xff;
  }
}



void MAC_File( vector <uint8_t> key, int size){
    Expand_Key(key);
    int kol = 0; 
    vector <uint8_t> r(8), rez(8), word(8), c(8), k1(8), k2(8), b(8);
    for (int i = 0; i<8; i++) { 
      c[i] = 0;
      b[i] = 0;
      r[i] = 0;
    }
    b[7] = 0x1b;
    while (size)
    {
        if (size > 8)
        {
          cout << size << "   ";
           read_file( word, 8, kol * 8);
           Add_8(word, c, c);
           Encrypt(c, c);
         //  write_file( c, 8, 8*kol);
           size = size-8;
        }
        else
        {
          Encrypt(r,r);
           if ((r[0] & 0xf0) < 0x80) mult2(r, k1); 
           else
           {
            mult2(r, k1);
            Add_8(k1,b, k1);
           }
            if ((k1[0] & 0xf0) < 0x80) mult2(k1, k2); 
           else
           {
            mult2(k1, k2);
            Add_8(k2,b, k2);
           }           
           read_file( word, size, kol * 8);
           if (size != 8) Proc_2(word, size, 8-size%8); 
    
           Add_8(word, c, c);
           if (size == 8)
            Add_8(c, k1, c); 
          else
            Add_8(c, k2, c);
          Encrypt(c,c);
          write_file( c, 4, 0);
          size = 0;
        }
        kol ++;
    }
}




void help_function(){
  cout << "GOST_R_3412-2015 for n = 64. Name : Magma\n";
  cout << "Mode : \n";
  cout << "-ecb : Electronic Codebook\n";
  cout << "-cbc : Cipher Block Chaining\n";
  cout << "-cfb : Cipher Feedback\n";
  cout << "-ofb : Output Feedback\n";
  cout << "-ctr : Counter\n";
  cout << "-mac : Message authentication codes\n";
  cout << "Operations : \n";
  cout << "-e encrypt\n-d decrypt\n";
  cout << "-k <keyfile> special key for cither\n";
  cout << "Options : \n";
  cout << "-v <iv file> file with IV value\n";
  cout << "-i <input.file> file with input data\n";
  cout << "-o <output.file> file with output data\n";
}




void input_handler(){
   if (!strcmp(argv_new[u], "-i")){
        if ( u+1 < argc_new){
          u++;
          input_filename = argv_new[u];
          u++;
        }
        else {
      perror("There is no input-file name");
      exit (0);
        }
      }
}



void output_handler(){
   if (!strcmp(argv_new[u], "-o")){
        if ( u+1 < argc_new){
          u++;
          output_filename = argv_new[u];
          u++;
        }
        else {
      perror("There is no output-file name");
      exit (0);
        }
      }
}

void key_handler (){
    if (!strcmp(argv_new[u], "-k")){
        if ( u+1 < argc_new){
          u++;
          key_filename = argv_new[u];
          u++;
          read_file_key(key_filename, keylong);  
        }
        else {
      perror("There is no key-file name");
      exit (0);
        }
      }
}



void iv_handler (){
    if (!strcmp(argv_new[u], "-v")){
        if ( u+1 < argc_new){
          u++;
          iv_filename = argv_new[u];
          u++;
        }
        else {
      perror("There is no iv-file name");
      exit (0);
        }
      }
}




void operation_handler (){
    if (!strcmp(argv_new[u], "-e")){
      flag_op = 1;
      u++;
    } else
    if (!strcmp(argv_new[u], "-d")){
      flag_op = 0;
      u++;
    }
}


void mac_handler (){
  mode_name = "mac";
  int j;
  while (u < argc_new){
    j = u;
    key_handler();
    if ( j == u ) input_handler();
    if ( j == u ) output_handler();
    if ( j == u ) {
      perror("wrong command. mode : mac");
      exit (0);
    }
  }
  filesize = get_file_size(input_filename);
}



void ecb_handler (){
  mode_name = "ecb";
  int j;
  while (u < argc_new  ){
    j = u;
    key_handler();
    if (j == u) input_handler();
    if (j == u) output_handler();
   
    if (u == j) operation_handler();
    if ( j == u ) {
      perror("wrong command. mode : ecb");
      exit (0);
    }
  }
  filesize = get_file_size(input_filename);
}



void ctr_handler (){
  mode_name = "ctr";
  int j;
  while (u < argc_new){
    j = u;
    key_handler();
    if (j == u) input_handler();
    if (j == u) output_handler();
    if (j == u) operation_handler();
    if (j == u) iv_handler();
    if ( j == u ) {
      perror("wrong command. mode : ctr");
      exit (0);
    }
  }
  filesize = get_file_size(input_filename);
}




void ofb_handler (){
  mode_name = "ofb";
  int j;
  while (u < argc_new){
    j = u;
    key_handler();
    if (j == u) input_handler();
    if (j == u) output_handler();
    if (j == u) operation_handler();
    if (j == u) iv_handler();
    if ( j == u ) {
      perror("wrong command. mode : ofb");
      exit (0);
    }
  }
  filesize = get_file_size(input_filename);
}



void cbc_handler (){
  mode_name = "cbc";
  int j;
  while (u < argc_new){
    j = u;
    key_handler();
    if (j == u) input_handler();
    if (j == u) output_handler();
    if (j == u) operation_handler();
    if (j == u) iv_handler();
    if ( j == u ) {
      perror("wrong command. mode : cbc");
      exit (0);
    }
  }
  filesize = get_file_size(input_filename);

}



void cfb_handler (){
  mode_name = "cfb";
  int j;
  while (u < argc_new){
    j = u;
    key_handler();
    if (j == u) input_handler();
    if (j == u) output_handler();
    if (j == u) operation_handler();
    if (j == u) iv_handler();
    if ( j == u ) {
      perror("wrong command. mode : cfb");
      exit (0);
    }
  }
  filesize = get_file_size(input_filename);
}



void checking (const char * key, int oper = -1){
  if (key == NULL){
    perror("There is no key");
    exit(0);
  }
  if (oper == -1){
    perror("There is no operation");
    exit(0);
  }
}




void help_handler(){
   if (argc_new == 2) {
      help_function();
      exit(0);
    } else {
      perror("wrong command. mode : help/h");
      exit (0);
    }
}


void macik(){
   mac_handler();
   checking(key_filename, 1);
   MAC_File(keylong, filesize);
}

void ctrik(){
    ctr_handler();
    checking(key_filename, flag_op);
    CTR_File(keylong, filesize); 
}

void ofbik(){
    ofb_handler();
    checking(key_filename,flag_op);
    OFB_File(keylong, filesize);
}


void ecbik(){
    ecb_handler();
    checking(key_filename, flag_op);
    if (flag_op == 1) 
      ECB_Encrypt_File(keylong, filesize);
    else
      ECB_Decrypt_File(keylong, filesize);
}


void cfbik(){
   cfb_handler();
   checking(key_filename, flag_op);
    if (flag_op == 1) 
      CFB_File_Encrypt(keylong, filesize);
    else
      CFB_File_Decrypt(keylong, filesize);
}

void cbcik(){
    cbc_handler();
    checking(key_filename, flag_op);
    if (flag_op == 1) 
      CBC_File_Encrypt(keylong, filesize);
    else
      CBC_File_Decrypt(keylong, filesize);
}


void sortik(){
  char * temp;
  for (int i = 1; i< argc_new; i++){
    if ((!strcmp(argv_new[i], "--mac")) || (!strcmp(argv_new[i], "--ctr")) 
      ||  (!strcmp(argv_new[i], "--ofb")) || (!strcmp(argv_new[i], "--ecb")) 
    || (!strcmp(argv_new[i], "--cfb")) ||
    (!strcmp(argv_new[i], "--cbc")) || (!strcmp(argv_new[i], "-h")) 
    || (!strcmp(argv_new[i], "--help"))){
      temp = argv_new[i]; 
      for (int j = i; j>1; j--)
        argv_new[j] = argv_new[j-1];
        argv_new[1] = temp;
        break;
    }
}
}


int main (int argc, char* argv[]){
  argc_new = argc;
  u = 2;
  for (int i = 1; i < argc_new; i++)
    argv_new[i] = argv[i];
  sortik();
  if ( (!strcmp(argv[1], "-h")) || (!strcmp(argv[1], "--help")) ){
      help_handler();
      return 0;
    } else
    if (!strcmp(argv_new[1], "--mac")) macik(); else
    if (!strcmp(argv_new[1], "--ctr")) ctrik(); else
    if (!strcmp(argv_new[1], "--ofb")) ofbik(); else
    if (!strcmp(argv_new[1], "--ecb")) ecbik(); else
    if (!strcmp(argv_new[1], "--cfb")) cfbik(); else
    if (!strcmp(argv_new[1], "--cbc")) cbcik(); else
      {
        perror("wrong mode");
        exit (0);
      }
}

