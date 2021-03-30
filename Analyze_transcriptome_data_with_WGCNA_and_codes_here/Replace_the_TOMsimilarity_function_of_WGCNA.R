### Before executing the following code, 
### you must have got the unsigned Adjacency Matrix named adjacency_matrix

### install and import the Rcpp amd RcppArmadillo packages
# install.packages( "Rcpp" , "RcppArmadillo" )
library(Rcpp)
library(RcppArmadillo)
RcppArmadillo.package.skeleton("FastCalculateTOM") # If you have run this line of code, there is already a folder named FastCalculateTOM in the working directory of R, then you don’t need to run this line of code. 

### Put the C++ source files in the current working directory of R, 
### then compile and import the C++ function
# getwd()  # Look at the current working directory 
Rcpp::sourceCpp( "C_OpenMP.cpp" )

### run the C++ function, and get the TOM
TOM = calculate_TOM_from_unsigned_Adjacency_Matrix( adjacency_matrix )