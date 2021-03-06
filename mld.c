#include<stdio.h>
#include "mld.h"
#include "css.h"
#include<assert.h>
#include<string.h>
#include<stdlib.h>
#include<memory.h>

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
    //printf("Total Structures: %d\n",struct_db->count);
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
void print_object_rec(object_db_rec_t *obj_rec, int i){
    if(!obj_rec) return;
    printf(ANSI_COLOR_MAGENTA "-----------------------------------------------------------------------------------------------------|\n"ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW "%-3d ptr = %-10p | next = %-10p | units = %-4d | struct_name = %-10s | is_root = %s |\n"ANSI_COLOR_RESET, 
        i, obj_rec->ptr, obj_rec->next, obj_rec->units, obj_rec->struct_rec->struct_name, obj_rec->is_root ? "TRUE " : "FALSE"); 
    printf(ANSI_COLOR_MAGENTA "-----------------------------------------------------------------------------------------------------|\n"ANSI_COLOR_RESET);
}

void print_object_db(object_db_t *object_db){
    object_db_rec_t *head = object_db->head;
    unsigned int i = 0;
    printf(ANSI_COLOR_CYAN "Printing OBJECT DATABASE\n" ANSI_COLOR_RESET);
    for(; head; head = head->next){
        print_object_rec(head, i++);
        //mld_dump_object_rec_detail(head);
    }
}

void mld_dump_object_rec_detail(object_db_rec_t *obj_rec){
    int n_fields = obj_rec->struct_rec->n_fields;
    field_info_t *field = NULL;
    int units = obj_rec->units, obj_index = 0,
    field_index = 0;
    for(; obj_index < units; obj_index++){
        char *current_object_ptr = (char *)(obj_rec->ptr) + 
                        (obj_index * obj_rec->struct_rec->ds_size);
        for(field_index = 0; field_index < n_fields; field_index++){
            field = &obj_rec->struct_rec->fields[field_index];
            switch(field->dtype){
                case UINT8:
                case INT32:
                case UINT32:
                    printf("%s[%d]->%s = %d\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, *(int *)(current_object_ptr + field->offset));
                    break;
                case CHAR:
                    printf("%s[%d]->%s = %s\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, (char *)(current_object_ptr + field->offset));
                    break;
                case FLOAT:
                    printf("%s[%d]->%s = %f\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, *(float *)(current_object_ptr + field->offset));
                    break;
                case DOUBLE:
                    printf("%s[%d]->%s = %f\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, *(double *)(current_object_ptr + field->offset));
                    break;
                case OBJ_PTR:
                    printf("%s[%d]->%s = %p\n", obj_rec->struct_rec->struct_name, obj_index, field->fname,  (void *)*(intptr_t *)(current_object_ptr + field->offset));
                    break;
                case OBJ_STRUCT:
                    /*Later*/
                    break;
                default:
                    break;
            }
        }
    }
}
static  object_db_rec_t* object_db_lookup(object_db_t* object_db,
                                            void* ptr
                                            ){
    object_db_rec_t* current_record =  object_db->head;
    while(current_record){
        if(current_record->ptr == ptr){
            return current_record;
        }
        current_record = current_record->next;
    }  
    return NULL;                              
}

static void add_object_to_db(object_db_t* object_db,
                             void* ptr,
                             int units,
                             struct_db_rec_t* struct_rec,
                             mld_boolean_t is_root
                            ){
    //Check if the object exists                            
    object_db_rec_t* obj_rec = object_db_lookup(object_db,ptr);
    if(obj_rec != NULL){
        printf("Object already exists in the database\n");
        assert(0);
    } 
    //Initializng the new object record
    obj_rec = (object_db_rec_t*)calloc(1,sizeof(object_db_rec_t));  
    obj_rec->ptr = ptr;
    obj_rec->struct_rec = struct_rec;
    obj_rec->next =NULL;
    obj_rec->units = units;
    obj_rec->is_root = is_root;

    //Add the object to the object DB linkedlist
    obj_rec->next = object_db->head;
    object_db->head = obj_rec;
    object_db->count++;

}
//removes record from db and returns the object
//xfree function takes care of freeing the record object
static object_db_rec_t* remove_object_from_db(object_db_t* object_db,
                             object_db_rec_t* obj_rec
                            ){
    //Check if the object is null
    // validation of if the object exists done in xfree                              
    if(obj_rec == NULL){
        assert(0);
    } 
    
    object_db_rec_t* deleted_obj = NULL;
    if(object_db->head == obj_rec){ //Node to be deleted is head
        deleted_obj = object_db->head;
        object_db->head = object_db->head->next;
        deleted_obj->next = NULL;
        return deleted_obj;
    }
    else{
        object_db_rec_t* current = object_db->head->next,*prev = object_db->head;
        while(current != NULL){
            if(current == obj_rec){
                prev->next = current->next;
                current->next = NULL;
                deleted_obj = current;
                return deleted_obj;
            }
            current = current->next;
            prev = prev->next;
        }
        
    }
    return NULL;

}

void* xcalloc(object_db_t* object_db,
            char* struct_name,
            int units
            ){
    struct_db_rec_t* struct_rec = struct_db_look_up(object_db->struct_db,
                                                    struct_name);
    assert(struct_rec);
    void* ptr = calloc(units,struct_rec->ds_size);
    add_object_to_db(object_db,ptr,units,struct_rec,MLD_FALSE);
    return ptr;
}

void xfree(object_db_t* object_db,void* ptr){
    //Check if the object exists                            
    object_db_rec_t* obj_rec = object_db_lookup(object_db,ptr);
    if(obj_rec == NULL){
        printf("Object does not exists in the database\n");
        assert(0);
    } 
    object_db_rec_t* deleted_obj = remove_object_from_db(object_db,obj_rec);
    free(deleted_obj->ptr);
    free(deleted_obj);
}

void mld_register_root_object(object_db_t* object_db,
                             void* objptr,
                            char* struct_name,
                            unsigned int units){
    
    struct_db_rec_t* struct_rec = struct_db_look_up(object_db->struct_db,struct_name);
    assert(struct_rec);
    add_object_to_db(object_db,objptr,units,struct_rec,MLD_TRUE);
}
void mld_set_dynamic_object_as_root(object_db_t* object_db,void* ptr){
    object_db_rec_t* obj_rec = object_db_lookup(object_db,ptr);
    assert(obj_rec);
    obj_rec->is_root = MLD_TRUE;
}

static void init_mld_algotithms(object_db_t* obj_db){
    object_db_rec_t* current = obj_db->head;
    while(current){
        current->is_visited = MLD_FALSE;
        current = current->next;
    }
}
static object_db_rec_t* get_next_root_object(object_db_t* object_db,object_db_rec_t* current_root){
    object_db_rec_t* current_rec = NULL;
    if(current_root == NULL){
        current_rec = object_db->head;
    }
    else{
        current_rec = current_root->next;
    }
    while(current_rec){
        if(current_rec->is_root == MLD_TRUE){
            return current_rec;
        }
        current_rec = current_rec->next;
    }
    return NULL;
}

static void mld_explore_objects_recursively(object_db_t* object_db,object_db_rec_t* parent_obj){
    char* parent_object_ptr = NULL,
        * child_obj_offset = NULL;
    void* child_object_address = NULL;
    field_info_t* field_info = NULL;
    //struct_db_rec_t* struct_rec = struct_db_look_up();
    object_db_rec_t* child_rec = NULL;
    struct_db_rec_t* parent_struct_rec = parent_obj->struct_rec;
    assert(parent_obj->is_visited == MLD_TRUE);
    for(int i = 0;i<parent_obj->units;i++){
        parent_object_ptr = (char*)parent_obj->ptr +(i*parent_struct_rec->ds_size);
        for(int n_fields = 0;n_fields < parent_struct_rec->n_fields;n_fields++){
            field_info = &parent_struct_rec->fields[n_fields];
            switch(field_info->dtype){
                case UINT8:
                case UINT32:
                case INT32:
                case DOUBLE:
                case FLOAT:
                case CHAR:
                case OBJ_STRUCT:
                            break;
                case OBJ_PTR:
                default:{
                    child_obj_offset = parent_object_ptr + field_info->offset;
                    memcpy(&child_object_address,child_obj_offset,sizeof(void*));
                    if(!child_object_address)
                        continue;
                    
                }
                child_rec = object_db_lookup(object_db,child_object_address);
                assert(child_rec);
                if(child_rec->is_visited == MLD_FALSE){
                    child_rec->is_visited = MLD_TRUE;
                    mld_explore_objects_recursively(object_db,child_rec);
                }
                else{
                    continue;
                }
                    
            }

        }
    }
}

void run_mld_algorithms(object_db_t* object_db){
    init_mld_algotithms(object_db);
    object_db_rec_t* root_obj = get_next_root_object(object_db,NULL);
    while(root_obj){
        if(root_obj->is_visited == MLD_TRUE){
            root_obj = get_next_root_object(object_db,root_obj);
            continue;
        }
        else{
            root_obj->is_visited = MLD_TRUE;
            mld_explore_objects_recursively(object_db,root_obj);
            root_obj = get_next_root_object(object_db,root_obj);
        }
    }

}

void report_leaked_objects(object_db_t *object_db){

    int i = 0;
    object_db_rec_t *head;

    printf("Dumping Leaked Objects\n");

    for(head = object_db->head; head; head = head->next){
        if(head->is_visited == MLD_FALSE){
            print_object_rec(head, i++);
            mld_dump_object_rec_detail(head);
            printf("\n\n");
        }
    }
}
/*Support for primitive data types*/
void mld_init_primitive_data_types_support(struct_db_t *struct_db){
    REG_STRUCT(struct_db, int , 0);
    REG_STRUCT(struct_db, float , 0);
    REG_STRUCT(struct_db, double , 0);
}