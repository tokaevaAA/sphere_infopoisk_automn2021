#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <regex>

#include <algorithm>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <locale>
#include <codecvt>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <chrono>


void write2predstavlenie_of7bit(int flag, char a, std::vector<char>& buffer){
    char otv=a;
    otv=otv^(flag<<7);
    buffer.push_back(otv);
}


void write2predstavlenie_anynum(int a, std::vector<char>& buffer){
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

class NodeStruct{
public:
    int mytype; //0=&,1=|,2=end,3=!end
    std::string mytoken;
    std::vector<int>* myvector;
    int tek_pos;
    bool flag;
    int podh_docid;
public:
    NodeStruct(int a,std::string s,std::vector<int>* v=nullptr,int p=0, bool fl=false, int pdid=-1){
        mytype=a;
        mytoken=s;
        myvector=v;
        tek_pos=p;
	flag=fl;
	podh_docid=pdid;
	
    }
    ~NodeStruct(){}
    void pechat(){
        std::cout<<mytype<<std::endl;
    }
};

template <class T>
class Tree;

template<class T>
class Node {
public:
    T data_;
    Node<T>* left_;
    Node<T>* right_;
public:
    Node(T data, Node<T>* left = nullptr, Node<T>* right = nullptr)
    : data_(data), left_(left), right_(right) {}

    int get_data() const { return data_; }
    void pechat(){data_.pechat();}

    friend class Tree<T>;

};

template<class T>
class Tree {
public:
    Node<T>* root_;
public:
    Tree() : root_(nullptr) {}
    Tree(Node<T>* new_root):root_(new_root){}
    // Destructor
    void deleteBranch(Node<T>* node) {
        if (!node) return;
        deleteBranch(node->left_);
        deleteBranch(node->right_);
        delete node;
    }
    ~Tree() { deleteBranch(root_); }

    void printtree(Node<T>* koren,int offset){
        if(koren==NULL) {return;}
        printtree(koren->right_,offset+1);
        for(int i=0;i<offset;i=i+1) printf("       ");
        koren->pechat();
        printtree(koren->left_,offset+1);
    }

    
    void pechat(){
        //printf("derevo ");
        printf("\n\n\n");
        printtree(root_,0);
        printf("\n\n\n");
    }
};



template<class T>
Node<T>* create_subtree(std::string stroka, std::unordered_map<std::string,std::vector<int> >& my_index, const std::vector<std::string>& v){
    //std::cout<<"HERE1 "<<stroka<<std::endl;
    if (stroka[0]=='(' && stroka[stroka.size()-1]==')'){
        bool need_to_delete_brackets=true;
        int cnt=1;
        for (int i=1; i<stroka.size()-1; i=i+1){
            if (stroka[i]=='('){cnt+=1;}
            if (stroka[i]==')'){cnt-=1;}
            if (cnt==0){
                need_to_delete_brackets=false;
                break;
            }
        }
        if (need_to_delete_brackets){
            stroka=stroka.substr(1,stroka.size()-2);
        }
    }
    //std::cout<<stroka<<std::endl;
        
    std::size_t ind_oper=std::min(stroka.find("&"),stroka.find("|"));
    std::size_t ind_otkr=stroka.find("(");
    
    if (ind_oper==std::string::npos && ind_otkr==std::string::npos){
	std::string new_stroka=*(stroka[0]!='!'?std::lower_bound(v.begin(),v.end(),stroka):std::lower_bound(v.begin(),v.end(),stroka.substr(1,stroka.size()-1)));
	
        
        
        T tek_oper=(stroka[0]!='!'?T(2,new_stroka,&my_index[new_stroka]):T(3,new_stroka,&my_index[new_stroka]));
        return new Node<T>(tek_oper,nullptr,nullptr);
    }
    
    int oper_type=(ind_oper==stroka.find("&")?0:1);
    //std::cout<<"HERE2 "<<stroka<<" "<<oper_type<<std::endl;
    

    if (ind_oper<ind_otkr){
        T tek_oper=T(oper_type,stroka);
        //std::cout<<"MYTYPE "<<tek_oper.mytype<<std::endl;
        return new Node<T>(tek_oper,create_subtree<T>(stroka.substr(0,ind_oper-1),my_index,v),
                           create_subtree<T>(stroka.substr(ind_oper+2,stroka.size()-ind_oper-2),my_index,v));
    }
    
    int cnt=1;
    std::size_t ind_zakr;
    for (int i=ind_otkr+1; i<stroka.size(); i=i+1){
        if (stroka[i]=='('){cnt+=1;}
        if (stroka[i]==')'){cnt-=1;}
        if (cnt==0){
            ind_zakr=i;
            break;
        }
    }
    ind_oper=ind_zakr+2;
    oper_type=(stroka[ind_oper] == '&'?0:1);
    
    T tek_oper=T(oper_type,stroka);
    return new Node<T>(tek_oper,create_subtree<T>(stroka.substr(1,ind_zakr-1),my_index,v),
                       create_subtree<T>(stroka.substr(ind_zakr+4,stroka.size()-ind_zakr-4),my_index,v));

}


std::tuple<std::unordered_map<std::string,std::vector<int> >, std::vector<std::string>> read_myindex(FILE* fin){
    std::unordered_map<std::string,std::vector<int> > my_index;
    std::vector<std::string> my_doc_id2url;

    int amt_docs;
    fscanf(fin, "%d",&amt_docs);
    //fin>>amt_docs;
    std::cout<<"amt_docs="<<amt_docs<<std::endl;
    my_doc_id2url.resize(amt_docs);
    int cnt=0;
    for (std::string& tek_url: my_doc_id2url){
        //fin>>tek_url;
        char tekstring[1000];
        fscanf(fin, "%s", tekstring);
        //printf("%s\n",tekchar);
        tek_url=std::string(tekstring);
        //tek_url.push_back(tekchar);
        
        //std::cout<<tek_url<<std::endl;
    }
    
    
    int amt_tokens;
    //fin>>amt_tokens;
    fscanf(fin, "%d",&amt_tokens);
    std::cout<<"amt_tokens="<<amt_tokens<<std::endl;
    for (int i=0; i<amt_tokens; i=i+1){
        std::string tek_token;
        char tekstring[1000];
        fscanf(fin, "%s", tekstring);
        //printf("%s\n",tekchar);
        tek_token=std::string(tekstring);
        //std::cout<<"tek_token="<<tek_token<<std::endl;
        //fin>>tek_token;
        //std::cout<<"tek_token="<<tek_token<<std::endl;
        //std::cout<<"HHhhHHH"<<std::endl;
        int buf_size;
        //fin>>buf_size;
        fscanf(fin, "%d",&buf_size);
        //std::cout<<"buf_size="<<buf_size<<std::endl;
        
        //std::cout<<"AAAaAAAA"<<std::endl;
        std::vector<char> buffer(buf_size);
        for (int i=0; i<buf_size; i=i+1){
            //fin>>el;
            char tekchar;
            int k;
            k=fscanf(fin, "%c",&tekchar);
            //printf("k=%d\n",k);
            buffer.push_back(tekchar);
        }
        char tekchar;
        fscanf(fin, "%c",&tekchar);
        //std::cout<<"BBBBBb"<<std::endl;
        //std::vector<char> v(buf_size);
        //for (int j = 0; j < buf_size; ++j) {v[j] = (char)(buffer[j]);}
        my_index[tek_token]=varbyte_decode(buffer);
        
    }
    return std::make_tuple(my_index, my_doc_id2url);

}

template<class T>
void process_node(Node<T>* node, int cur_id){
    if (node->data_.myvector!=nullptr){

	if (node->data_.mytype == 2){
		//while(node->data_.tek_pos < node->data_.myvector->size() && (*(node->data_.myvector))[node->data_.tek_pos]<cur_id){
	    	//node->data_.tek_pos+=1;
		//}
		auto it = std::lower_bound(node->data_.myvector->begin() + node->data_.tek_pos, node->data_.myvector->end(), cur_id);
		node->data_.tek_pos = std::distance(node->data_.myvector->begin(), it);

		if (node->data_.tek_pos==node->data_.myvector->size()){
	    	node->data_.flag=false;
	    	node->data_.podh_docid=-1;
		}
		else{
	    	node->data_.flag=((*(node->data_.myvector))[node->data_.tek_pos]==cur_id);
	    	node->data_.podh_docid=(*(node->data_.myvector))[node->data_.tek_pos];
		}
	}
	else{
	     if (cur_id==-1){cur_id=0;}
	     //while(node->data_.tek_pos < node->data_.myvector->size() && (*(node->data_.myvector))[node->data_.tek_pos] < cur_id){
	     //	node->data_.tek_pos+=1;
	     //}
	     auto it = std::lower_bound(node->data_.myvector->begin() + node->data_.tek_pos, node->data_.myvector->end(), cur_id);
	     node->data_.tek_pos = std::distance(node->data_.myvector->begin(), it);


	     if (node->data_.tek_pos == node->data_.myvector->size() || 
			(node->data_.tek_pos < node->data_.myvector->size() && (*(node->data_.myvector))[node->data_.tek_pos] > cur_id )){ 
		  node->data_.flag=true;
	    	  node->data_.podh_docid=cur_id;
	     }
	     else{
		 int tmp=cur_id;
	         while(node->data_.tek_pos < node->data_.myvector->size() && (*(node->data_.myvector))[node->data_.tek_pos] == tmp ){
		     tmp+=1;
	             node->data_.tek_pos+=1;
		 }
		 node->data_.flag=false;
	    	 node->data_.podh_docid=tmp;
	     }
		
	}
	
    }
    else{
	process_node(node->left_, cur_id);
        process_node(node->right_, cur_id);
	if (node->data_.mytype==0){
	    node->data_.flag=node->left_->data_.flag & node->right_->data_.flag;
	    if (node->left_->data_.podh_docid==-1 || node->right_->data_.podh_docid==-1){
		node->data_.podh_docid=-1;
	    }
	    else{
	        node->data_.podh_docid=std::max(node->left_->data_.podh_docid, node->right_->data_.podh_docid);
	    }
	    
	}

	if (node->data_.mytype==1){
	    node->data_.flag=node->left_->data_.flag | node->right_->data_.flag;
	    if (node->left_->data_.podh_docid==-1 || node->right_->data_.podh_docid==-1){
		node->data_.podh_docid=std::max(node->left_->data_.podh_docid, node->right_->data_.podh_docid);

	    }
	    else{
	        node->data_.podh_docid=std::min(node->left_->data_.podh_docid, node->right_->data_.podh_docid);
	    }
	    
	}
        
	
    }


}

template<class T>
std::vector<int> get_res_docs(Tree<T>& Mytree, std::size_t my_doc_id2url_size ){
   std::vector<int> res{};
   do   {
        process_node(Mytree.root_, Mytree.root_->data_.podh_docid);
	if (Mytree.root_->data_.flag==1 && Mytree.root_->data_.podh_docid < my_doc_id2url_size){
	   res.push_back(Mytree.root_->data_.podh_docid);
	   Mytree.root_->data_.podh_docid+=1;

	}
        //std::cout<<Mytree.root_->data_.flag<<std::endl;
        //std::cout<<Mytree.root_->data_.podh_docid<<std::endl;
   }while (Mytree.root_->data_.podh_docid !=-1 && Mytree.root_->data_.podh_docid < my_doc_id2url_size);


       
    return res;

   
}

template<class T>
std::vector<int> dfs_for_node(Node<T>* node, int my_index_size ){
    if (node->data_.mytype == 2){
        return *(node->data_.myvector);
    }
    else if (node->data_.mytype == 3){
        std::vector<int> v;
        for (int i=0; i<(*(node->data_.myvector))[0]; i=i+1){
            v.push_back(i);
        }
        for (int i=1; i<node->data_.myvector->size(); i=i+1){
            for (int j=(*(node->data_.myvector))[i-1]+1; j<(*(node->data_.myvector))[i]; j=j+1){
                v.push_back(j);
            }
        }
        for (int i=node->data_.myvector->back()+1; i<my_index_size; i=i+1){
            v.push_back(i);
        }
        return v;
    }
    else if (node->data_.mytype == 0){
        auto left = dfs_for_node(node->left_, my_index_size);
        auto right = dfs_for_node(node->right_, my_index_size);
        int pos_left=0;
        int pos_right=0;
        std::vector<int> v;
        while (pos_left<left.size()){
            auto lower_bound = std::lower_bound(right.begin() + pos_right, right.end(), left[pos_left]);
            if (*lower_bound == left[pos_left]) { v.push_back(*lower_bound); }
            pos_right = std::distance(right.begin(), lower_bound);
            pos_left+=1;
        }
        return v;
    }
    
    auto left = dfs_for_node(node->left_, my_index_size);
    auto right = dfs_for_node(node->right_, my_index_size);
    int pos_left=0;
    int pos_right=0;
    std::vector<int> v;
    while (pos_left<left.size() && pos_right<right.size()  ){
        v.push_back(left[pos_left] < right[pos_right] ? left[pos_left++] : right[pos_right++]);
    }
    std::copy(left.begin() + pos_left, left.end(), std::back_inserter(v));
    std::copy(right.begin() + pos_right, right.end(), std::back_inserter(v));
    return v;
    
}

int main(){
    
    printf("Hello\n");
    
    

    //std::ifstream fin("indexCompress.txt");
    FILE* fin=fopen("indexCompress.txt","r");
    std::unordered_map<std::string,std::vector<int> > my_index;
    std::vector<std::string> my_doc_id2url;
    
    std::tie(my_index, my_doc_id2url) = read_myindex(fin);
    fclose(fin);
    
    std::vector<std::string> v;
    for( const auto& [key, value] : my_index ) {
    v.push_back(key);
    }
    std::sort(v.begin(), v.end());
    std::cout<<"SizeMyindex="<<v.size()<<std::endl;
   
    


    //std::string zapros="(власти & (бельгии | парижа)) & !теракт";
    //std::string zapros="путин & медведев";
    std::string zapros="путин & (медведев | (бельгии | парижа))";
    //std::string zapros="участников & митинга";
    //std::string zapros="(участников & митинга) | бренда";
    

    

    Tree<NodeStruct> Mytree(create_subtree<NodeStruct>(zapros,my_index,v));
    Mytree.pechat();

    
    
    std::chrono::steady_clock::time_point time_begin_nepotok = std::chrono::steady_clock::now();
    std::vector<int> res=dfs_for_node(Mytree.root_, my_index.size());
    std::chrono::steady_clock::time_point time_end_nepotok = std::chrono::steady_clock::now();
    auto duration_nepotok=std::chrono::duration_cast<std::chrono::nanoseconds> (time_end_nepotok - time_begin_nepotok).count();
    std::cout<<"duration_nepotok="<<duration_nepotok<<"ns"<<std::endl;
    
    
    std::chrono::steady_clock::time_point time_begin_potok = std::chrono::steady_clock::now();
    res=get_res_docs(Mytree,my_doc_id2url.size());
    std::chrono::steady_clock::time_point time_end_potok = std::chrono::steady_clock::now();
    auto duration_potok=std::chrono::duration_cast<std::chrono::nanoseconds> (time_end_potok - time_begin_potok).count();
    std::cout<<"duration_potok="<<duration_potok<<"ns"<<std::endl;
    
    
    std::cout<<zapros<<std::endl;
    std::cout<<res.size()<<std::endl;
    for (auto el:res){
	    std::cout<<my_doc_id2url[el]<<std::endl;
    }
    



    

    


    
    printf("Goodbuy\n");


    return 0;
    
}



