#include <fstream>
#include <iostream>
#include <sstream>

#include <nlohmann/json.hpp>
#include <libSif/ASTAnalyser.hpp>
#include <libUtils/Constants.hpp>
#include <libUtils/Utils.hpp>
#include <cxxopts.hpp>

int main(int argc, char** argv){
    std::string ast_file_name, ast_json_file_name, output_file_name;
    try{
        cxxopts::Options options("Sif", "Solidity Source Code Instrumentation Framework");
        options.add_options()
            ("a,ast", "AST in plain text file name", cxxopts::value<std::string>())
            ("j,json", "AST in compact JSON format file name", cxxopts::value<std::string>())
            //("b,abi", "ABI file name", cxxopts::value<std::string>())
            ("o,output","Output file name", cxxopts::value<std::string>())
            ("h,help", "Print help message");
        auto result = options.parse(argc, argv);
        if (result.count("help")) {
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(Sif::ErrorCode::SUCCESS);
        }
        if (result.count("ast")) {
            ast_file_name = result["ast"].as<std::string>();
        } else {
            std::cout << "Plain-text AST file not provided\n";
            exit(Sif::ErrorCode::TEXT_AST_FILE_NOT_PROVIDED);
        }
        if (result.count("json")) {
            ast_json_file_name = result["json"].as<std::string>();
        } else {
            std::cout << "JSON AST file not provided\n";
            exit(Sif::ErrorCode::JSON_AST_FILE_NOT_PROVIDED);
        }
        //if (result.count("abi")) {
        //    abi_file_name = result["abi"].as<std::string>();
        //} else {
        //    std::cout << "ABI file not provided\n";
        //    exit(Sif::ErrorCode::ABI_FILE_NOT_PROVIDED);
        //}
    } catch (const cxxopts::OptionException& e) {
        std::cout << "error parsing options: " << e.what() << "\n";
        exit(Sif::ErrorCode::COMMAND_LINE_ARGUMENTS_ERROR);
    }

    std::ifstream ast_text_file_stream(ast_file_name), ast_json_file_stream(ast_json_file_name);
    std::stringstream ast_text_stream, abi_stream;
    std::string new_line, sol_name, ast_json_content;

    while (getline(ast_text_file_stream, new_line)) {
        ast_text_stream << new_line << "\n";
    }

    while (getline(ast_json_file_stream, new_line)) {
        if (new_line.find(".sol =======") != std::string::npos) {
            sol_name = Sif::Utils::substr_by_edge(new_line, "======= ", " =======");
            break;
        }
    }
    while (getline(ast_json_file_stream, new_line)) {
        if (new_line.find(".sol =======") == std::string::npos) {
            ast_json_content = ast_json_content + new_line + "\n";
        } else {
            nlohmann::json ast_json = nlohmann::json::parse(ast_json_content);
            //Sif::ASTAnalyser ast_analyser(ast_json, sol_name);
            //std::stringstream new_source = ast_analyser.analyse();
            std::cout << sol_name << " " << ast_json.at("absolutePath") << std::endl;
            sol_name = Sif::Utils::substr_by_edge(new_line, "", "");
            ast_json_content = "";
        }
    }
    if (ast_json_content != "") {
        nlohmann::json ast_json = nlohmann::json::parse(ast_json_content);
        Sif::ASTAnalyser ast_analyser(ast_text_stream, ast_json, true, sol_name);
        std::stringstream new_source = ast_analyser.analyse();
        std::cout << sol_name << " " << ast_json.at("absolutePath") << std::endl;
        if (output_file_name != "") {
            std::ofstream output_file_stream(output_file_name);
            output_file_stream << new_source.str();
            output_file_stream.close();
        } else {
            std::cout << new_source.str() << std::endl;
        }
    }
    
    ast_json_file_stream.close();

    exit(Sif::ErrorCode::SUCCESS);

    


    /*
    Code debugging the json ast
    std::cout << jsonast.dump();
    std::cout << jsonast.at("nodes").at(2).at("nodes").at(0).at("visibility") << std::endl;
    std::cout << jsonast.at("nodes").at(2).at("nodes").at(0).at("visibility") << std::endl;
    std::cout << jsonast.at("nodes").at(8).at("nodes").at(5).at("name") << std::endl;
    std::cout << jsonast.at("nodes").at(8).at("nodes").at(5).at("visibility") << std::endl;
    std::cout << jsonast.at("nodes").at(8).at("nodes").at(5).at("stateMutability") << std::endl;
    std::cout << jsonast.at("nodes").at(8).at("nodes").at(4).at("isConstructor") << std::endl;
    std::cout << jsonast.at("nodes").at(7).at("nodes").at(5).at("visibility") << std::endl;
    */

    
    
    

    return 0;
}