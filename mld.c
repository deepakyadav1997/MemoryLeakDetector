#include<stdio.h>
#include "mld.h"
#include "css.h"
#include<assert.h>
#include<string.h>

char *DATA_TYPE[] = {"UINT8", "UINT32", "INT32",
                     "CHAR", "OBJ_PTR", "VOID_PTR", "FLOAT",
                     "DOUBLE", "OBJ_STRUCT"};


int add_structure_to_struct_db(struct_db_t* struct_db,struct_db_rec_t* struct_rec){
    struct_rec->next = struct_db->head;
    struct_db->head = struct_rec;
    struct_db->count++; 
    return 0;
}
struct_db_rec_t* struct_db_look_up(struct_db_t *struct_db, char *struct_name){
    if(!struct_db)
        return NULL;
    struct_db_rec_t *current_record = struct_db->head;
    printf("Total Structures: %d\n",struct_db->count);
    while(current_record){
        if(strcmp(current_record->struct_name,struct_name) == 0)
            return current_record;
        current_record = current_record->next;
    }
    return NULL;
}

//Printing functions
void print_structure_rec(struct_db_rec_t* struct_rec){
    if(!struct_rec) return;
        int j = 0;
    field_info_t *field = NULL;
    printf(ANSI_COLOR_BLUE "|------------------------------------------------------|\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "| %-20s | size = %-8d | #flds = %-3d |\n" ANSI_COLOR_RESET, struct_rec->struct_name, struct_rec->ds_size, struct_rec->n_fields);
    printf(ANSI_COLOR_BLUE "|------------------------------------------------------|------------------------------------------------------------------------------------------|\n" ANSI_COLOR_RESET);
    for(j = 0; j < struct_rec->n_fields; j++){
        field = &struct_rec->fields[j];
        printf("  %-20s |", "");
        printf("%-3d %-20s | dtype = %-15s | size = %-5d | offset = %-6d|  nstructname = %-20s  |\n",
                j, field->fname, DATA_TYPE[field->dtype], field->size, field->offset, field->nested_str_name);
        printf("  %-20s |", "");
        printf(ANSI_COLOR_BLUE "--------------------------------------------------------------------------------------------------------------------------|\n" ANSI_COLOR_RESET);
    }
}
void print_structure_db(struct_db_t* struct_db){
    if(!struct_db)
        return;
    struct_db_rec_t *current_record = struct_db->head;
    printf("Total Structures: %d\n",struct_db->count);
    while(current_record){
        print_structure_rec(current_record);
        current_record = current_record->next;
    }
}
