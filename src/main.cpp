#include <iostream>
#include <sstream>
#include <optional>
#include <vector>
#include "fstream"

#include "generation.h"


int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "cdash <input.cd>" << std::endl;
        return EXIT_FAILURE;
    }


    std::string contents;
    {
        std::stringstream file_contents_stream;
        std::fstream input(argv[1], std::ios::in);
        file_contents_stream << input.rdbuf();
        contents = file_contents_stream.str();
    }

    Tokenizer tokenizer(std::move(contents));

    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(std::move(tokens));
    std::optional<NodeProgram> program = parser.parse_program();
    if (!program.has_value())
    {
        std::cerr<<"Invalid program"<<std::endl;
        exit(EXIT_FAILURE);
    }
    Generator generator(program.value());
    {
        std::fstream file("out.asm",std::ios::out);
        file << generator.generate_program();
    }

    system("nasm -felf64 out.asm");
    system("ld out.o -o out");

    return EXIT_SUCCESS;
}
