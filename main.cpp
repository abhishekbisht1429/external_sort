#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>

static const int DEFAULT_BUFFER_SIZE = 134217728;
static const std::string TEMP_DIR = "../.temp";

/*
 * NOTE: next_int sets the eof bit after
 * reading the last number which is unlike
 * the std libraries
 */
int next_int(std::ifstream& in) {
    char c;
    std::string str;
    while((in>>c) && (c!=','))
        str += c;
    return std::stoll(str);
}

/* merge 2 sorted files */
std::string merge_files(const std::string& fpath1, const std::string& fpath2) {
    /* open file streams */
    std::ifstream if1(fpath1);
    std::ifstream if2(fpath2);
    std::string out_file_name = fpath1+"merge_file";
    std::ofstream of(out_file_name);

    bool read_from_if1, read_from_if2;
    int num1, num2;

    /* write the first num without a leading comma */
    if(if1 && if2) {
        num1 = next_int(if1);
        num2 = next_int(if2);
        if (num1 < num2) {
            of << num1;
            read_from_if1 = true;
            read_from_if2 = false;
        } else {
            of << num2;
            read_from_if2 = true;
            read_from_if1 = false;
        }
    } else if(if1) {
        of<<next_int(if1);
    } else {
        of<<next_int(if2);
    }

    /* write rest of the numbers */
    while(if1 && if2) {
        if(read_from_if1) num1 = next_int(if1);
        if(read_from_if2) num2 = next_int(if2);

        if(num1 < num2) {
            of << "," << num1;
            read_from_if1 = true;
            read_from_if2 = false;
        } else {
            of << "," << num2;
            read_from_if2 = true;
            read_from_if1 = false;
        }
    }

    /* write remaining numbers */
    while(if1)
        of<<","<<next_int(if1);

    while(if2)
        of<<","<<next_int(if2);

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



std::vector<std::string> split(std::string fpath, size_t in_memory_buf_size,
                               std::string out_dir_path) {
    std::ifstream in(fpath);
    int *buf = nullptr;
    try {
        buf = new int[in_memory_buf_size];
    } catch(std::bad_alloc &ba) {
        std::cout<<"failed to allocate memory for buffer";
        return {};
    }

    std::vector<std::string> fpaths;
    int i=0;
    int fid = 0;
    while(in) {
        buf[i++] = next_int(in);
        if(i == in_memory_buf_size || in.eof()) {
            /* Sort the buffer before storing on disk */
            std::sort(buf, buf+i);

            std::string fpath = out_dir_path+"/"+std::to_string(fid);
            std::ofstream out(fpath);
            fpaths.push_back(fpath);

            if(i>0) out<<buf[0];
            for(int j=1; j<i; ++j)
                out<<","<<buf[j];

            i=0;
            out.close();
            ++fid;
        }
    }

    delete[] buf;

    return fpaths;
}

int main(int argc, char **argv) {
    auto start_time = std::chrono::high_resolution_clock::now();
    /* ------------------  Code Start ---------------------------- */
    if(argc < 3) {
        std::cout<<"invalid number of args\n";
        return 1;
    }
    std::string input_file = argv[1];
    std::string output_file = argv[2];
    size_t buf_size = DEFAULT_BUFFER_SIZE;
    std::string output_dir = TEMP_DIR;
    if(argc >= 4)
        buf_size = (1024 * 1024 * std::stoll(argv[3])) / sizeof (int);
    if(argc >= 5)
        output_dir = argv[4];


    std::vector<std::string> fpaths = split(input_file, buf_size, output_dir);
    std::string sorted_fpath = merge_files(fpaths, 0, fpaths.size()-1);
    rename(sorted_fpath.c_str(), output_file.c_str());

    /* ------------------- Code End -------------------------------*/
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>
            (end_time - start_time);
    long long ns = duration.count();
    double s = ns/1e9;

    std::cout<<"Duration: "<<s<<" sec\n";
    return 0;
}
