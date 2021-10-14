#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <regex>
#include <map>


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


std::string beautify_token(std::string tek_token) {

	static std::map<std::string, std::string> small2big = {
		{"а", "А"}, {"б", "Б"}, {"в", "В"}, {"г", "Г"}, {"д", "Д"},
		{"е", "Е"}, {"ё", "Ё"}, {"ж", "Ж"}, {"з", "З"}, {"и", "И"},
		{"й", "Й"}, {"к", "К"}, {"л", "Л"}, {"м", "М"}, {"н", "Н"},
		{"о", "О"}, {"п", "П"}, {"р", "Р"}, {"с", "С"}, {"т", "Т"},
		{"у", "У"}, {"ф", "Ф"}, {"х", "Х"}, {"ц", "Ц"}, {"ч", "Ч"},
		{"ш", "Ш"}, {"щ", "Щ"}, {"э", "Э"}, {"ю", "Ю"}, {"я", "Я"}
	};

    std::regex vowel_re(",|[.]|[*]|\"|\'|[?]|[(]|[)]|!|-|;|:|»");
    tek_token = std::regex_replace(tek_token, vowel_re, "");

    for (auto& [small, big] : small2big) {
        if (tek_token.substr(0, 2) == big) {
            tek_token = small + (tek_token.substr(2, tek_token.size() - 2));
		}
    }
    
    return tek_token.substr(0, tek_token.size() - 4);
}

void doc2url_and_fill_index(std::ifstream& f,
	std::unordered_map<std::string,std::vector<int> >& my_index,
	std::vector<std::string>& my_doc_id2url) {
    
    std::string tek_line;
    std::regex tek_regex("\\s+");

    int doc_id = my_doc_id2url.size();
    while (std::getline(f, tek_line)) {
        
        if (tek_line.find("http://lenta.ru") != std::string::npos) {

              doc_id += 1;

              std::size_t pos_h = tek_line.find("http");
              std::size_t pos_sl = tek_line.find_last_of("/");
              std::size_t pos_pr = tek_line.substr(pos_h + 3, tek_line.size() - pos_h - 3).find_first_of(" ");
              if (pos_pr != std::string::npos) {
                pos_pr += pos_h + 3;
              }
              std::size_t pos = std::min(pos_sl, pos_pr);

              my_doc_id2url.push_back(tek_line.substr(pos_h, pos - pos_h));
        }

		// std::cout << "tek_size=" << my_index.size() << std::endl;

		std::sregex_token_iterator tek_iterator(tek_line.begin(), tek_line.end(), tek_regex, -1);
		std::sregex_token_iterator tek_end;
		while (tek_iterator != tek_end) {
			std::string tek_token = *tek_iterator;
			if (tek_token.size() <= 6 || tek_token < "А" || tek_token > "я") {
				tek_iterator++;
				continue;
			}

			tek_token = beautify_token(tek_token);

			if (my_index[tek_token].empty() || my_index[tek_token].back() < doc_id - 1) {
				my_index[tek_token].push_back(doc_id - 1);
			}

			tek_iterator++;
		}
    }
}


int main(){
    
    printf("Hello\n");
    
    std::unordered_map<std::string,std::vector<int> > my_index;
    std::vector<std::string> my_doc_id2url;
    std::vector<std::string> input_mas = {"dataset1.txt", "dataset2.txt","dataset3.txt", "dataset4.txt","dataset5.txt", "dataset6.txt","dataset7.txt", "dataset8.txt",};

	for (std::string input_name : input_mas) {
    	std::ifstream fin(input_name);
    	doc2url_and_fill_index(fin, my_index, my_doc_id2url);
    }

    FILE* fout=fopen("indexCompress.txt","w");

    fprintf(fout,"%lu\n", my_doc_id2url.size());
    //fout << my_doc_id2url.size() << std::endl;

    for (const std::string tek_url : my_doc_id2url) {
        for (int i=0; i<tek_url.size(); i=i+1){
            fprintf(fout,"%c", tek_url.c_str()[i]);
        }
        fprintf(fout,"\n");
    }

    fprintf(fout,"%lu\n", my_index.size());
    //fout << my_index.size() << std::endl;
    //std::cout << my_index.size() << std::endl;

    for (const auto& [tek_token, tek_mas] : my_index) {
        //fout << tek_token << std::endl;
        for (int i=0; i<tek_token.size(); i=i+1){
            fprintf(fout,"%c", tek_token.c_str()[i]);
        }
        fprintf(fout,"\n");
        

        std::vector<char> buffer;
        varbyte_encode(tek_mas, buffer);
        
        fprintf(fout,"%lu\n", buffer.size());
        
        for (int i=0; i<buffer.size(); i=i+1){
            fprintf(fout,"%c", buffer[i]);
        }
        fprintf(fout,"\n");
        
        
    }
    
    fclose(fout);
	printf("Goodbuy\n");
   
    return 0; 
}
