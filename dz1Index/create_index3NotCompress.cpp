#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <regex>
#include <map>

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

    std::ofstream fout("indexNotCompress.txt");

    fout << my_doc_id2url.size() << std::endl;

    for (const auto& tek_url : my_doc_id2url) {
        fout << tek_url << std::endl;
        
    }

    fout << my_index.size() << std::endl;

    for (const auto& [tek_token, tek_mas] : my_index) {
        fout << tek_token << std::endl;
        fout << tek_mas.size() << std::endl;

        for (int d : tek_mas) {
            fout << d << " ";
        }
        fout << std::endl;
    }

	printf("Goodbuy\n");
   
    return 0; 
}
