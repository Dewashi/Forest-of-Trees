/*Created by Deevesh Pillay Shanmuganathan
Creates a map of trees to simulate the possible consequences of
differing amounts of rainfall on the ability of a grove of trees to survive*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_TREES 52                      /*Max number of trees per tsv file*/
#define NUM_HEADING_LINES 1                  /*Number of newlines in heading*/
#define GRID_HEIGHT 60            /*Grid max y value*/
#define GRID_WIDTH 70                   /*Grid max x value*/
#define X_AXIS_LABEL_INTERVAL 0.5   /*label offset value from an x value*/
#define Y_AXIS_LABEL_INTERVAL 1.0   /*label offset value from an y value*/
#define LOWEST_X_LABEL 1        /*Lowest label on x axis*/
#define LOWEST_Y_LABEL 2            /*Lowest label on y axis*/
#define UNREACHABLE_INDEX -1 /*Used to determine if no tree index was accessed*/
#define PRINT_AXIS_INTERVAL 10 /*Interval between the plotting of axis labels*/
#define STRESS_FACTOR_DEATH_VALUE 1.0 /*Value at which trees die*/
#define STAGE_3_VALUE 3       /*Used as an inputs to determine stage num when*/
#define STAGE_4_VALUE 4       /*using create_map*/
#define METERS_PER_TALL_CELL 2  /*Num of meter per tall cell*/
#define LOWEST_DIGIT_NUMBER 0  /*A number witht he lowest number of digits*/
#define NEW_DIGIT_FACTOR 10   /*When multiplied by it, this number adds one
                                digit to a number*/


typedef struct{                /*struct containing tree attributes*/
    char label;
    double xloc;
    double yloc;
    int liters;
    double rootrad;
    int alive_status;  /*has the value 1 if a tree is alive and 0 if dead*/
    int tree_count;/*Count of the number of a certain tree on a map*/
} tree_t;

void print_x_axis(int stage);
void print_y_axis(int first_call, int stage, int row_num);
int mygetchar();
int input_data(tree_t *tree_array);
void display_root_conflicts(tree_t *tree_array,int num_trees);
double calc_tree_distance(double xloc1, double xloc2,
double yloc1, double yloc2);
void create_map(int stage,int map_required, tree_t *tree_array,int num_trees);
int find_closest_tree(double xloc, double yloc,
tree_t tree_array[],int num_trees);
void calc_tree_survival(tree_t *tree_array,int num_trees,
double rain_amount, int first_call);
void initialise_tree_count(tree_t *tree_array,int num_trees);
int calc_stressed_tree(tree_t *tree_array, int num_trees,
double rain_amount);
int calc_digit_length(int row_num);
void print_axis_space(int row_num);


/*runs the show*/
int main(int argc, char** argv){
    int num_trees;
    /*Declares an array of tree structs*/
    tree_t tree_array[MAX_TREES];
    /*num_trees returned to be used as a buddy variable*/
    num_trees = input_data(tree_array);
    display_root_conflicts(tree_array, num_trees);
    create_map(STAGE_3_VALUE,1, tree_array, num_trees);
    /*Stage 4 is only computed if there is
     a provided rainfall amount that is above 0*/
    if (atof(argv[1]) > 0){
        calc_tree_survival(tree_array, num_trees, atof(argv[1]), 1);
    }
    return 0;
}


/*Based on code written by Alistair Moffat on
 https://people.eng.unimelb.edu.au/ammoffat/teaching/20005/ass2/
 Gets a character and stores it in a variable*/
int mygetchar(){
    int c;
    while ((c=getchar())=='\r') {
	}
	return c;
}


/*The Majority of this function is derived from Stage 1 of the first project,
created by Deevesh Pillay Shanmuganathan*/
/*S1: Fills up an array of structs with tree data*/
int input_data(tree_t *tree_array){
    int num_trees=0, count_newline=0, total_water=0;
    char next_char;
    /*Scans and discards the heading of the input TSV file
    until the end of line 1 has been reached*/
    while(count_newline != NUM_HEADING_LINES){
        next_char= mygetchar();
        if (next_char == '\n'){
            count_newline += 1;
        }
        if (count_newline == NUM_HEADING_LINES){
            break;
        }
    }
    /*Stores scanned data from a input TSV file into structs in an array,
     reading them in a set format*/
    while(scanf(" %c%lf%lf%d%lf", &tree_array[num_trees].label,
    &tree_array[num_trees].xloc, &tree_array[num_trees].yloc,
    &tree_array[num_trees].liters, &tree_array[num_trees].rootrad) == 5){
        total_water += tree_array[num_trees].liters;
        /*Assigns the default state of all trees*/
        tree_array[num_trees].alive_status = 1;
        tree_array[num_trees].tree_count = 0;
        num_trees += 1;
    }
    /*print the required output for S1*/
    printf("\nS1: total data lines   =    %2d trees\n", num_trees);
    printf("S1: total water needed = %.3f megaliters per year\n",
    total_water/1000000.0);
    printf("\n");
    return(num_trees);
}


/*S2: Compares each tree with other trees and finds out if their*/
 /*roots intersect */
void display_root_conflicts(tree_t *tree_array,int num_trees){
    int i, j;
    double tree_distance;
    for(i=0; i<num_trees; i++){
        printf("S2: tree %c is in conflict with",tree_array[i].label );
        for(j=0; j<num_trees; j++){
            tree_distance = calc_tree_distance(tree_array[i].xloc,
            tree_array[j].xloc, tree_array[i].yloc, tree_array[j].yloc);
            /*gets the distance between both trees and determines if their
            roots intersect based on the sum of root radi*/
            /*The guard tree_distance != 0 is set to make
             sure a tree isn't compared to itself*/
            if ((tree_distance < tree_array[i].rootrad +
            tree_array[j].rootrad) && tree_distance != 0){
                printf(" %c",tree_array[j].label );
            }
        }
        printf("\n");
    }
    printf("\n");
}


/*Calculates the distance between two trees*/
double calc_tree_distance(double xloc1, double xloc2,
double yloc1, double yloc2){
    double tree_x_difference, tree_y_difference;
    tree_x_difference = xloc1 - xloc2;
    tree_y_difference = yloc1 - yloc2;
    /*Applies the distance formula between two trees*/
    return (sqrt((tree_x_difference * tree_x_difference) +
    (tree_y_difference * tree_y_difference)));
}


/*S3 & S4:Creats the map for stage 3 and 4*/
void create_map(int stage,int map_required, tree_t *tree_array,int num_trees){
    int i, j, tree_index, first_y_axis_call = 1, max_height=GRID_HEIGHT;
    double  xloc, yloc;
    /*Checks to see if the supplied grid height is odd, and if so adjusts
     y values so that a map can be plotted*/
    if (GRID_HEIGHT % 2 == 1){
        max_height = GRID_HEIGHT - 1;
    }
    for(i=max_height; i>=LOWEST_Y_LABEL; i-=2){
    /*Determines if a map is to be created or not and if so adds axis labels*/
        if (map_required == 1){
            print_y_axis(first_y_axis_call, stage, i-METERS_PER_TALL_CELL);
        }
        /*Determines the point at which a tree label is to be plotted*/
        /*and uses these points as input to find the closest tree*/
        for(j=LOWEST_X_LABEL; j<GRID_WIDTH; j++){
            yloc = i - Y_AXIS_LABEL_INTERVAL;
            xloc = j - X_AXIS_LABEL_INTERVAL;
            tree_index = find_closest_tree(xloc, yloc, tree_array, num_trees);
            /*If no tree label is associated with a certain point*/
            /*and if a map is to be created a space character is plotted*/
            if ((tree_index == UNREACHABLE_INDEX) && (map_required == 1)){
                printf(" ");
            }
            /*Determines if a map is to be createD or not
            and if so plots tree labels*/
            else if(map_required == 1){
                printf("%c",tree_array[tree_index].label);
            }
        }
        /*Determines if a map is to be create or not and if
        so creates newlines for each row of the grid*/
        if (map_required == 1){
            printf("\n");
        }
        /*this triggers when the top of the y axis is plotted and prevents
         it from plotting again*/
        first_y_axis_call = 0;
    }
    /*Determines if a map is to be create or not and if so create the x axis*/
    if (map_required == 1){
        print_x_axis(stage);
    }
}


/*S3 & s4: Finds the closest tree to a set of co-ordinates*/
int find_closest_tree(double xloc, double yloc,
tree_t tree_array[],int num_trees){
    int i, first_tree=1, tree_index=UNREACHABLE_INDEX;
    double closest_point, tree_distance;
    for(i=0; i<num_trees; i++){
        tree_distance = calc_tree_distance(xloc,
        tree_array[i].xloc, yloc,tree_array[i].yloc);
        /*Gets the distance between a tree and a set of co-ordinates and if
         that distance is smaller than the root radius of a tree, the tree
         details are stored*/
        /*The tree must be alive to account for stage 4, in which
         dead trees cannot be plotted*/
        if ((tree_distance < tree_array[i].rootrad)
        && (tree_array[i].alive_status == 1)){
            if(first_tree == 1){
                /*Initialises closest point to have a value based on the first
                tree that meets the conditions*/
                closest_point  = tree_distance;
                tree_index = i;
                first_tree = 0;
            }
            /*If another tree is closer to a pair of
            co-ordinates than the current tree and it is alive, the index of
            that tree and it's distance to the point is recorded*/
            if (tree_distance < closest_point){
                closest_point = tree_distance;
                tree_index = i;
            }
        }
    }
    /*Increments the numer of spaces taken up on the grid by a specific tree*/
    tree_array[tree_index].tree_count += 1;
    /*returns the index of the tree which is closest
    to the supplied co-ordinates)*/
    return(tree_index);
}


/*S3 & S4: Prints the appropriate axis labels for each row of the y axis*/
void print_y_axis(int first_call, int stage, int row_num){
    printf("S%d:",stage);
    /*Prints the top label of the y axis once*/
    if (first_call == 1){
        printf(" %d +\n",GRID_HEIGHT);
        printf("S%d:",stage);
    }
    /*Numbers are only printed at an interval of 10 digits on the y axis*/
    if(row_num % PRINT_AXIS_INTERVAL == 0){
        print_axis_space(row_num);
        printf("%d +",row_num);
    }
    else{
        print_axis_space(LOWEST_DIGIT_NUMBER);
        printf("  |");
    }
}


/*Calculates the number of digits in a number, used to determine the numbers
of extra space required to shift the grid right if there are too many digits
in the max grid height*/
int calc_digit_length(int row_num){
    int digit_threshold = NEW_DIGIT_FACTOR, num_digits = 1;
    /*determines number of digits by comparing numbers against a multiplicative
    threshold*/
    while(digit_threshold <= row_num){
        num_digits += 1;
        digit_threshold *= NEW_DIGIT_FACTOR;
    }
    return num_digits;
}


/*Adds space needed to shift the grid right to account for numbers with
many digits along the y axis if a large GRID_HEIGHT value is used*/
void print_axis_space(int row_num){
    int num_digits, max_digit, i;
    num_digits = calc_digit_length(row_num);
    max_digit = calc_digit_length(GRID_HEIGHT);
    /*Finds the differece between the numbers of digits of the longest number
    and another number and adds spaces to the shorter number to make up for it*/
    for (i=0; i<=max_digit-num_digits; i++){
        printf(" ");
    }
}


/*S3 & S4: Prints the appropriate axis labels for each column of the x axis*/
void print_x_axis(int stage){
    int column_num;
    printf("S%d:",stage);
    print_axis_space(LOWEST_DIGIT_NUMBER);
    printf("   ");
    for(column_num=0; column_num<=GRID_WIDTH; column_num++){
            /*"+"is only printed at an interval of 10 digits on the x axis,*/
            /* "-" is printed otherwise"*/
        if(column_num % PRINT_AXIS_INTERVAL == 0){
            printf("+");
        }
        else{
            printf("-");
        }
    }
    /*0 is printed seperately as it is always included on the map, it
    also allows other numbers to be formatted more conveniently*/
    printf("\nS%d:",stage);
    print_axis_space(LOWEST_DIGIT_NUMBER);
    printf("   0");
    for(column_num=1; column_num<=GRID_WIDTH ;column_num++){
        /*A column number is only printed if the modulus of it is 0*/
        if(column_num % PRINT_AXIS_INTERVAL == 0){
            printf("%10d",column_num);
        }

    }
}


/*S4: Finds the order in which stressed trees dies
and plots a map of surviving trees through a recursive function*/
void calc_tree_survival(tree_t *tree_array, int num_trees,
double rain_amount, int first_call){
    int stressed_index=0;
    /*Prints a statement about the rainfall amount on the
    first call to this function*/
    if (first_call == 1){
        printf("\n\n\nS4: rainfall amount = %.1f\n",rain_amount);
        first_call = 0;
    }
    stressed_index = calc_stressed_tree(tree_array ,num_trees, rain_amount);
    /*If stressed_index = -1 it means that there are no more stressed trees,
    allowing a map of surviving trees to be plotted*/
    if (stressed_index == UNREACHABLE_INDEX){
        printf("\n");
        /*Creates a map of surviving trees and ends the recursive function*/
        create_map(STAGE_4_VALUE, 1, tree_array, num_trees);
        return;
    }
    /*Changes the status of the most overstressed tree to 0 (dead)*/
    tree_array[stressed_index].alive_status = 0;
    /*resets the tree count of trees on a map, allowing them to be recaluclated
     on the next call to this function to find the next stressed tree*/
    initialise_tree_count(tree_array, num_trees);
    /*recalulates the number of squares taken by trees on a map,
    excluding dead trees, a map is not created in this case*/
    create_map(STAGE_4_VALUE, 0, tree_array, num_trees);
    /*This function is returned until no trees are stressed, at which point*/
    /* a map of surviving trees are plotted*/
    return calc_tree_survival(tree_array, num_trees, rain_amount, 0);
}


/*S4: Finds out if there are any stressed trees
 and if so identifies the most stressed tree*/
int calc_stressed_tree(tree_t *tree_array,
int num_trees, double rain_amount){
    int i, stressed_index=UNREACHABLE_INDEX;
    double required_rainfall, stress_factor, stress_record = 0;
    /*calculates the stress factor of a tree if they appear on the map*/
    for(i=0; i<num_trees; i++){
        if(tree_array[i].tree_count != 0){
            required_rainfall = 1.0 * tree_array[i].liters
            / (tree_array[i].tree_count * METERS_PER_TALL_CELL);
            stress_factor = required_rainfall / rain_amount;
            /*Compares the stress factors of any stressed trees
            and records the most stressed tree*/
            if((stress_factor >= STRESS_FACTOR_DEATH_VALUE) &&
            (stress_factor >= stress_record)){
                stress_record = stress_factor;
                stressed_index = i;
            }
        }
    }
    /*If there are any stressed trees a statement is printed about that tree*/
    if (stressed_index != UNREACHABLE_INDEX){
        printf("S4: tree %c has stress factor %.2f and dies next\n",
        tree_array[stressed_index].label, stress_record);
    }
    /*returns the index of the most stressed tree, if there are no stressed
    trees -1 is returned instead to indicate that no more trees will die*/
    return stressed_index;
}


/*S4: Resets the record of the map_space taken by a tree*/
void initialise_tree_count(tree_t *tree_array, int num_trees){
    int i;
    for(i=0; i< num_trees; i++){
        tree_array[i].tree_count = 0;
    }
}

/*programming is fun*/
