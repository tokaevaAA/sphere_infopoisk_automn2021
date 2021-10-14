#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

void write2predstavlenie_of7bit(int flag, char a, std::vector<char>& buffer){
    char otv=a;
    otv=otv^(flag<<7);
    buffer.push_back(otv);
}


void write2predstavlenie_anynum(int a, std::vector<char>& buffer){
    printf("%d %d %d\n",a, a%128, a>>7);
    if (a<=127){
        write2predstavlenie_of7bit(1, a, buffer);
    }
    else{
        write2predstavlenie_of7bit(0, a%128, buffer);
        write2predstavlenie_of7bit(1, a>>7, buffer);
    }
}


void varbyte_encode(std::vector<int> v, std::vector<char>& buffer){
    if (v.size()==0){return;}
    write2predstavlenie_anynum(v[0], buffer);
    for (int i=1; i<=v.size()-1; i=i+1){
        write2predstavlenie_anynum(v[i]-v[i-1], buffer);
    }
}

bool read_of7bit(int* multiply_by, int* otv, std::vector<char>& buffer, int* offset){
    
    *otv=(buffer[*offset]&((1<<7)-1));
    bool flag=buffer[*offset]&(1<<7);
    *offset+=1;
    *otv=(*otv)*(*multiply_by);
    return flag;
}

std::vector<int> varbyte_decode(std::vector<char>& buffer){
    std::vector<int> v;
    int offset=0;
    bool flag;
    int tek_sum=0;
    int tek_el=0;
    int multiply_by=1;
    while (offset<buffer.size()){
        flag=read_of7bit(&multiply_by, &tek_el, buffer, &offset);
        if (flag==false){
            tek_sum+=tek_el;
            multiply_by*=128;
        }
        else{
            if (v.size()==0){v.push_back(tek_sum+tek_el);}
            else{v.push_back(tek_sum+tek_el+v.back());}
            tek_sum=0;
            multiply_by=1;
        }
        
    }
    return v;
}

int main(){
    printf("Hello!\n");
    
    
    std::vector<char> buffer;
    std::vector<int> in_vector={9999};
    varbyte_encode(in_vector, buffer);
    
    std::cout<<"buffer.size= "<<buffer.size()<<std::endl;
    
    for (int i=0; i<buffer.size(); i=i+1){
        for (int j=7;j>=0;j=j-1){
            printf("%d ",(buffer[i]&(1<<j))/(1<<j));
        }
        printf("\n");
        
    }
    
    std::vector<int> out_vector=varbyte_decode(buffer);
    for (int el:out_vector){
        std::cout<<el<<std::endl;
    }
    

    printf("Goodbuy!\n");
    return 0;



}
