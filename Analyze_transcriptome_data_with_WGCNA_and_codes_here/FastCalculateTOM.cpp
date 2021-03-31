// [[Rcpp::depends(RcppArmadillo)]]
#include <cmath>
#include <omp.h>
#include <RcppArmadillo.h>

using namespace Rcpp;
using namespace std;

// [[Rcpp::export]]
Rcpp::NumericMatrix  calculate_unsigned_TOM_from_unsigned_or_signed_Adjacency_Matrix( Rcpp::NumericMatrix  Adjacency_Matrix_Rcpp_NumericMatrix )
{
    long int total_gene_number = Adjacency_Matrix_Rcpp_NumericMatrix.nrow();
    double ** array_2_dimension_adjacency = new double * [ total_gene_number ];
    for( long int i = 0 ; i<= total_gene_number-1 ; i++)
    {
        array_2_dimension_adjacency[i] = new double[total_gene_number];
    }

    // Assign the Adjacency_Matrix_Rcpp_NumericMatrix to the C++ two-dimensional array named array_2_dimension_adjacency
    #pragma omp parallel
    {
        #pragma omp single
        {
            for( long int row_index = 0 ; row_index <= total_gene_number - 1 ; row_index++ )
            {
                #pragma omp task
                {
                    for( long int column_index = 0 ; column_index <= total_gene_number - 1 ; column_index++ )
                    {
                        array_2_dimension_adjacency [ row_index ] [ column_index ] = Adjacency_Matrix_Rcpp_NumericMatrix( column_index , row_index );
                    }
                }
            }
        }
    }

    // calculate TOM
    double ** array_2_dimension_TOM = new double * [ total_gene_number ];
    for( long int i = 0 ; i <= total_gene_number-1 ; i++)
    {
        array_2_dimension_TOM[ i ]  =  new double[ total_gene_number ];
    }

    double * sums_of_each_row_in_an_array = new double [ total_gene_number ];
    #pragma omp parallel proc_bind( spread )
    {
        #pragma omp for schedule( static )
        for( long int current_row_index = 0 ; current_row_index <= total_gene_number - 1 ; current_row_index++  )
        {
            sums_of_each_row_in_an_array[ current_row_index ] = 0;
            for( long int current_column_index = 0 ; current_column_index <= total_gene_number - 1 ; current_column_index++  )
            {
                sums_of_each_row_in_an_array[ current_row_index ] = sums_of_each_row_in_an_array[ current_row_index ] + array_2_dimension_adjacency[ current_row_index ][ current_column_index ];
            }
        }
    }

    double * multiply_and_sum_of_combination_of_i_j_Change_triangular_matrix_to_an_array = new double [ (total_gene_number*(total_gene_number-1))/2 ];
    #pragma omp parallel proc_bind( spread )
    {
        #pragma omp single
        {
            long int how_many_table_cells_have_been_computed_in_triangular_matrix = 0;
            for( long int current_row_index = 0 ; current_row_index <= total_gene_number - 2 ; current_row_index++ )
            {
                long int how_many_table_cells_should_be_computed_in_current_row = 0 ;
                if( current_row_index == 0 )
                {
                    how_many_table_cells_have_been_computed_in_triangular_matrix = 0;
                }
                else
                {
                    how_many_table_cells_should_be_computed_in_current_row = total_gene_number - (current_row_index-1) - 1;
                    how_many_table_cells_have_been_computed_in_triangular_matrix = how_many_table_cells_have_been_computed_in_triangular_matrix + how_many_table_cells_should_be_computed_in_current_row;
                }
                #pragma omp task
                {
                    for( long int a_later_row_index_behind_current_row = current_row_index + 1 ; a_later_row_index_behind_current_row <= total_gene_number - 1 ; a_later_row_index_behind_current_row++  )
                    {
                        double current_i_j_combination_value = 0.0;
                        for( long int current_column_index = 0 ; current_column_index <= total_gene_number-1 ; current_column_index++)
                        {
                            current_i_j_combination_value  =  current_i_j_combination_value  +  array_2_dimension_adjacency[ current_row_index ] [ current_column_index ]  *  array_2_dimension_adjacency[ a_later_row_index_behind_current_row ][ current_column_index ] ;
                        }
                        long int index_of_current_i_j_combination_value = how_many_table_cells_have_been_computed_in_triangular_matrix - 1  +  ( a_later_row_index_behind_current_row - current_row_index );
                        multiply_and_sum_of_combination_of_i_j_Change_triangular_matrix_to_an_array[ index_of_current_i_j_combination_value ] = current_i_j_combination_value;
                    }
                }
            }
        }
    }

    #pragma omp parallel proc_bind( spread )
    {
        #pragma omp single
        {
            long int how_many_table_cells_have_been_computed_in_triangular_matrix = 0;
            for( long int current_row_index = 0 ; current_row_index <= total_gene_number - 2 ; current_row_index++ )
            {
                long int how_many_table_cells_should_be_computed_in_current_row = 0 ;
                if( current_row_index == 0 )
                {
                    how_many_table_cells_have_been_computed_in_triangular_matrix = 0;
                }
                else
                {
                    how_many_table_cells_should_be_computed_in_current_row = total_gene_number - (current_row_index-1) - 1;
                    how_many_table_cells_have_been_computed_in_triangular_matrix = how_many_table_cells_have_been_computed_in_triangular_matrix + how_many_table_cells_should_be_computed_in_current_row;
                }
                #pragma omp task
                {
                    for( long int current_column_index = current_row_index + 1 ; current_column_index <= total_gene_number - 1 ; current_column_index++  )
                    {
                        long int index_of_current_i_j_combination_value = how_many_table_cells_have_been_computed_in_triangular_matrix - 1  +  ( current_column_index - current_row_index );
                        double xigema_a_iu_a_ju =  multiply_and_sum_of_combination_of_i_j_Change_triangular_matrix_to_an_array[ index_of_current_i_j_combination_value ];
                        double xigema_a_iu = sums_of_each_row_in_an_array[ current_row_index ];
                        double xigema_a_ju = sums_of_each_row_in_an_array[ current_column_index ];
                        double a_ij = array_2_dimension_adjacency[ current_row_index ][ current_column_index ];
                        double value_of_TOM_current_row_column = ( xigema_a_iu_a_ju - a_ij ) / ( min( xigema_a_iu , xigema_a_ju ) - a_ij ) ;
                        array_2_dimension_TOM[ current_row_index ][ current_column_index ] = value_of_TOM_current_row_column;
                        array_2_dimension_TOM[ current_column_index ][ current_row_index ] = value_of_TOM_current_row_column;
                    }
                }
            }
        }
    }

    #pragma omp parallel proc_bind( spread )
    {
        #pragma omp for schedule( static )
        for( long int row_index = 0 ; row_index <= total_gene_number -1 ; row_index++  )
        {
            array_2_dimension_TOM[ row_index ][ row_index ] = 1.0;
        }
    }

    // release memory
    for(  long int row_index_of_array_2_dimension_adjacency = 0  ;  row_index_of_array_2_dimension_adjacency <= total_gene_number - 1  ;  row_index_of_array_2_dimension_adjacency++  )
    {
        delete [ ]   array_2_dimension_adjacency[ row_index_of_array_2_dimension_adjacency ];
    }
    delete [ ] array_2_dimension_adjacency ;
    delete [ ] sums_of_each_row_in_an_array ;
    delete [ ] multiply_and_sum_of_combination_of_i_j_Change_triangular_matrix_to_an_array ;

    Rcpp::NumericMatrix  TOM_Rcpp_NumericMatrix( total_gene_number , total_gene_number ) ;
    #pragma omp parallel
    {
        #pragma omp single
        {
            for( long int row_index = 0 ; row_index <= total_gene_number - 1 ; row_index++ )
            {
                #pragma omp task
                {
                    for( long int column_index = 0 ; column_index <= total_gene_number - 1 ; column_index++ )
                    {
                        TOM_Rcpp_NumericMatrix ( column_index , row_index ) = array_2_dimension_TOM [ row_index ] [ column_index ];
                    }
                }
            }
        }
    }

    for(  long int row_index_of_array_2_dimension_TOM = 0  ;  row_index_of_array_2_dimension_TOM <= total_gene_number - 1  ;  row_index_of_array_2_dimension_TOM++  )
    {
        delete [ ]   array_2_dimension_TOM[ row_index_of_array_2_dimension_TOM ];
    }
    delete [ ] array_2_dimension_TOM ;

    return TOM_Rcpp_NumericMatrix;
}
