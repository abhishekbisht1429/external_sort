#include <iostream>
#include <fstream>
#include <vector>

/* merge 2 sorted files */
std::string merge_files(const std::string& fpath1, const std::string& fpath2) {
    /* open file streams */
    std::ifstream if1(fpath1);
    std::ifstream if2(fpath2);
    std::string out_file_name = fpath1+"merge_file";
    std::ofstream of(out_file_name);

    /* read the files in memory */
    std::vector<int> vec1, vec2;
    int num;
    while(if1>>num)
        vec1.push_back(num);

    while(if2>>num)
        vec2.push_back(num);

    /* merge the sorted vectors */
    std::vector<int> vec3;
    int i=0, j=0, n=vec1.size(), m=vec2.size();
    while(i<n && j<m) {
        if(vec1[i] < vec2[j])
            vec3.push_back(vec1[i++]);
        else
            vec3.push_back((vec2[j++]));
    }

    while (i < n)
        vec3.push_back(vec1[i++]);

    while (j < m)
        vec3.push_back(vec2[j++]);

    /* write the sorted vector to the output stream */
    for(int num : vec3)
        of<<num<<"\n";

    if1.close();
    if2.close();
    of.close();

    /* delete the input files */
    remove(fpath1.c_str());
    remove(fpath2.c_str());

    /* rename the output file */
    rename(out_file_name.c_str(), fpath2.c_str());
    
    return fpath2;
}

std::string merge_files(std::vector<std::string>& file_names, int lo, int hi) {
    int size = hi-lo+1;
    if(size < 1)
        return "";
    if(size == 1)
            return file_names[lo];

    int mid = (lo+hi)/2;
    std::string fpath1 = merge_files(file_names, lo, mid);
    std::string fpath2 = merge_files(file_names, mid+1, hi);
    
    return merge_files(fpath1, fpath2);
}

long long next_ll(std::ifstream& in) {
    char c;
    std::string str;
    while((in>>c) && (c!=','))
        str += c;
    return std::stoll(str);
}

std::vector<std::string> split(std::string fpath, int in_memory_buf_size = 524280,
                               std::string out_dir_path="../.temp") {
    std::ifstream in(fpath);
    std::vector<int> buf(in_memory_buf_size);
    std::vector<std::string> fpaths;
    int i=0;
    int fid = 0;
    while(in) {
        buf[i++] = next_ll(in);
        if(i == in_memory_buf_size || in.eof()) {
            std::string fpath = out_dir_path+"/"+std::to_string(fid);
            std::ofstream out(fpath);
            fpaths.push_back(fpath);
            while(i>0)
                out<<buf[--i]<<"\n";
            out.close();
            ++fid;
        }
    }

    return fpaths;
}

int main() {
    std::vector<std::string> fpaths = split("../.temp/unsorted_file.txt");

    return 0;
}
