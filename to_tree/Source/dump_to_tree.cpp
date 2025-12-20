#include "tree.h"

#define COL_NEXT_BACKGROUND    "#7df879ff"
#define COL_LEFT_BACKGROUND    "#f879e7ff"
#define COL_CURRENT_BACKGROUND "#d3cd77ff"

#define COL_OP_BACKGROUND      "#26cfdb9f"
#define COL_VAR_BACKGROUND     "#d6ba7fbe"
#define COL_NUM_BACKGROUND     "#ac7fd6be"

#define COL_VALUE_BACKGROUND   "#7f8cd6be"

#define COL_RIGHT_ARROW        "#0ea30eff"
#define COL_LEFT_ARROW         "#fa2821ff"

static FILE *html_stream = nullptr;

static const char *dump_file_position  = "DUMP/dump.tex";
static const int kMaxFileNameLen       = 24;

static void creat_html(int num_call, dump_position position);
static void creat_dot(int num_call, code_tree_t tree);
static void print_dump_elem(FILE *stream_out, node_t *elem);

static void start_dump();
static void end_dump();

void show_dump(code_tree_t expression, dump_position position){
    static int num_call = 1;

    if (num_call == 1){
        start_dump();
        atexit(end_dump);
    }

    creat_dot(num_call, expression);
    creat_html(num_call, position);
    printf("\n____________\n");
    num_call++;
}

static void creat_html(int num_call, dump_position position){
    if (!html_stream) return;

    fprintf(html_stream, "\\section*{Dump called from %s:%d from func %s. The %d call}\n",
         position.file, position.line, position.func, num_call);

    fprintf(html_stream,  "\\begin{figure}[!h]"
                                         "\\begin{center}\n"
                                         "    \\includegraphics[width=16cm]{%d.png}\n"
                                         "    \\begin{center}\n"
                                         "    \\end{center}\n"
                                         "    \\label{graphic1b}\n"
                                         "\\end{center}\n"
                                         "\\end{figure}\n", num_call);
    fprintf(html_stream,  "\\newpage");
}

static void creat_dot(int num_call, code_tree_t expression){
    char file_name[kMaxFileNameLen] = {};
    sprintf(file_name, "DUMP/%d.dot", num_call);

    FILE *stream_out = fopen(file_name ,"w");
    if (!stream_out) return;

    fprintf(stream_out, "digraph structs {\n");
    if (expression){
        print_dump_elem(stream_out, expression);
    }
    fprintf(stream_out, "}\n");
    fclose(stream_out);
}

static void print_dump_elem(FILE *stream_out, node_t *elem){
    if (!elem) return;

    fprintf(stream_out, 
        "%lu [shape=\"plaintext\", label=<\n"
        "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n", (unsigned long)elem);

    fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_CURRENT_BACKGROUND "\" COLSPAN=\"2\">%p</TD></TR>\n", elem);

    switch(elem->type){
        case OP:
            fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_OP_BACKGROUND "\" COLSPAN=\"2\">%s</TD></TR>\n", "OP");

            for (size_t pos_op_list = 0; pos_op_list < op_list_size; pos_op_list++){
                if (op_list[pos_op_list].op == elem->val.op){
                    fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_VALUE_BACKGROUND "\" COLSPAN=\"2\">%s</TD></TR>\n", op_list[pos_op_list].for_dump);
                }
            }
            break;

        case VAR:
            fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_VAR_BACKGROUND   "\" COLSPAN=\"2\">%s</TD></TR>\n", "VAR");
            fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_VALUE_BACKGROUND "\" COLSPAN=\"2\">%s</TD></TR>\n", elem->val.var);
            break;

        case NUM:
            fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_NUM_BACKGROUND   "\" COLSPAN=\"2\">%s</TD></TR>\n", "NUM");
            fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_VALUE_BACKGROUND "\" COLSPAN=\"2\">%d</TD></TR>\n", elem->val.num);
            break;

        default:
            break;
    }
        
    fprintf(stream_out,
        "    <TR>\n"
        "        <TD BGCOLOR=\"" COL_LEFT_BACKGROUND    "\"> %p </TD>\n"
        "        <TD BGCOLOR=\"" COL_NEXT_BACKGROUND    "\"> %p </TD>\n"
        "    </TR>\n"
        "</TABLE>\n"
        ">];\n", elem->left_node, elem->right_node);

    if (elem->left_node){
        fprintf(stream_out, "%lu->%lu[color=\"" COL_LEFT_ARROW  "\"];\n", (unsigned long)elem, (unsigned long)elem->left_node);
        print_dump_elem(stream_out, elem->left_node);
    }

    if (elem->right_node){
        fprintf(stream_out, "%lu->%lu[color=\"" COL_RIGHT_ARROW "\"];\n", (unsigned long)elem, (unsigned long)elem->right_node);
        print_dump_elem(stream_out, elem->right_node);
    }
}

static void start_dump(){
    html_stream = fopen(dump_file_position,"w");
    fprintf(html_stream, "\\documentclass[a4paper,10pt]{article}\n"
                                        "\\usepackage[T2A]{fontenc}\n"
                                        "\\usepackage{graphicx}\n"
                                        "\\usepackage{float}\n"
                                        "\\begin{document}\n");
}   

static void end_dump(){
    if (!html_stream) return;

    fprintf(html_stream,  "\\end{document}");
                        
    fclose(html_stream);
}