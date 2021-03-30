#include <cmath>
#include <omp.h>

using namespace std;

// The two-dimensional array storing the unsigned Adjacency Matrix in the heap area of memory must be obtained first.
// This two-dimensional array is a secondary pointer, the data type is double. The unsigned Adjacency Matrix which must be a symmetric matrix, and the value of the diagonal is equal to 1.

// Then pass the secondary pointer and the number of nodes of undirected scale-free hierarchical weighted networks as parameters to the following function.
// The return value of the function is also a two-dimensional array, which stores the unsigned Topological Overlap Matrix.

double **  calculate_TOM_from_unsigned_Adjacency_Matrix( double **  array_2_dimension_adjacency , long int total_nodes_number )
{
    // calculate TOM
    double ** array_2_dimension_TOM = new double * [ total_nodes_number ];
    for( long int i = 0 ; i <= total_nodes_number-1 ; i++)
    {
        array_2_dimension_TOM[ i ]  =  new double[ total_nodes_number ];
    }

    double * sums_of_each_row_in_an_array = new double [ total_nodes_number ];
    #pragma omp parallel proc_bind( spread )
    {
        #pragma omp for schedule( static )
        for( long int current_row_index = 0 ; current_row_index <= total_nodes_number - 1 ; current_row_index++  )
        {
            sums_of_each_row_in_an_array[ current_row_index ] = 0;
            for( long int current_column_index = 0 ; current_column_index <= total_nodes_number - 1 ; current_column_index++  )
            {
                sums_of_each_row_in_an_array[ current_row_index ] = sums_of_each_row_in_an_array[ current_row_index ] + array_2_dimension_adjacency[ current_row_index ][ current_column_index ];
            }
        }
    }

    double * multiply_and_sum_of_combination_of_i_j_Change_triangular_matrix_to_an_array = new double [ (total_nodes_number*(total_nodes_number-1))/2 ];
    #pragma omp parallel proc_bind( spread )
    {
        #pragma omp single
        {
            long int how_many_table_cells_have_been_computed_in_triangular_matrix = 0;
            for( long int current_row_index = 0 ; current_row_index <= total_nodes_number - 2 ; current_row_index++ )
            {
                long int how_many_table_cells_should_be_computed_in_current_row = 0 ;
                if( current_row_index == 0 )
                {
                    how_many_table_cells_have_been_computed_in_triangular_matrix = 0;
                }
                else
                {
                    how_many_table_cells_should_be_computed_in_current_row = total_nodes_number - (current_row_index-1) - 1;
                    how_many_table_cells_have_been_computed_in_triangular_matrix = how_many_table_cells_have_been_computed_in_triangular_matrix + how_many_table_cells_should_be_computed_in_current_row;
                }
                #pragma omp task
                {
                    for( long int a_later_row_index_behind_current_row = current_row_index + 1 ; a_later_row_index_behind_current_row <= total_nodes_number - 1 ; a_later_row_index_behind_current_row++  )
                    {
                        double current_i_j_combination_value = 0.0;
                        for( long int current_column_index = 0 ; current_column_index <= total_nodes_number-1 ; current_column_index++)
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
            for( long int current_row_index = 0 ; current_row_index <= total_nodes_number - 2 ; current_row_index++ )
            {
                long int how_many_table_cells_should_be_computed_in_current_row = 0 ;
                if( current_row_index == 0 )
                {
                    how_many_table_cells_have_been_computed_in_triangular_matrix = 0;
                }
                else
                {
                    how_many_table_cells_should_be_computed_in_current_row = total_nodes_number - (current_row_index-1) - 1;
                    how_many_table_cells_have_been_computed_in_triangular_matrix = how_many_table_cells_have_been_computed_in_triangular_matrix + how_many_table_cells_should_be_computed_in_current_row;
                }
                #pragma omp task
                {
                    for( long int current_column_index = current_row_index + 1 ; current_column_index <= total_nodes_number - 1 ; current_column_index++  )
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
        for( long int row_index = 0 ; row_index <= total_nodes_number -1 ; row_index++  )
        {
            array_2_dimension_TOM[ row_index ][ row_index ] = 1.0;
        }
    }

    // release memory
    for(  long int row_index_of_array_2_dimension_adjacency = 0  ;  row_index_of_array_2_dimension_adjacency <= total_nodes_number - 1  ;  row_index_of_array_2_dimension_adjacency++  )
    {
        delete [ ]   array_2_dimension_adjacency[ row_index_of_array_2_dimension_adjacency ];
    }
    delete [ ] array_2_dimension_adjacency ;
    delete [ ] sums_of_each_row_in_an_array ;
    delete [ ] multiply_and_sum_of_combination_of_i_j_Change_triangular_matrix_to_an_array ;

    return array_2_dimension_TOM;
}

