#include "../pcq.h"

int main(int argc, char *argv[]) {

  pcq_parser_t *Input  = pcq_new("input");
  pcq_parser_t *Node  = pcq_new("node");
  pcq_parser_t *Leaf  = pcq_new("leaf");
  pcq_ast_t *ast, *tree, *child, *child_sub, *ast_next;
  pcq_ast_trav_t *trav;
  pcq_result_t r;
  int index, lb, i;

  pcqa_lang(PCQA_LANG_PREDICTIVE,
        " node : '(' <node> ',' /foo/ ',' <node> ')' | <leaf>;"
        " leaf : /bar/;"
        " input : /^/ <node> /$/;",
        Node, Leaf, Input, NULL);

  if (argc > 1) {

    if (pcq_parse_contents(argv[1], Input, &r)) {
      ast = r.output;
    } else {
      pcq_err_print(r.error);
      pcq_err_delete(r.error);
      pcq_cleanup(3, Node, Leaf, Input);
      return EXIT_FAILURE;
    }

  } else {

    if (pcq_parse_pipe("<stdin>", stdin, Input, &r)) {
      ast = r.output;
    } else {
      pcq_err_print(r.error);
      pcq_err_delete(r.error);
      pcq_cleanup(3, Node, Leaf, Input);
      return EXIT_FAILURE;
    }

  }

  /* Get index or child of tree */
  tree = ast->children[1];

  index = pcq_ast_get_index(tree, "node|>");
  child = pcq_ast_get_child(tree, "node|>");

  if(child == NULL) {
    pcq_cleanup(3, Node, Leaf, Input);
    pcq_ast_delete(ast);
    return EXIT_FAILURE;
  }

  printf("Index: %d; Child: \"%s\"\n", index, child->tag);

  /* Get multiple indexes or children of trees */
  index     = pcq_ast_get_index_lb(child, "node|leaf|regex", 0);
  child_sub = pcq_ast_get_child_lb(child, "node|leaf|regex", 0);

  while(index != -1) {
    printf("-- Index: %d; Child: \"%s\"\n", index, child_sub->tag);

    lb = index + 1;
    index     = pcq_ast_get_index_lb(child, "node|leaf|regex", lb);
    child_sub = pcq_ast_get_child_lb(child, "node|leaf|regex", lb);
  }

  /* Traversal */
  printf("Pre order tree traversal.\n");
  trav = pcq_ast_traverse_start(ast, pcq_ast_trav_order_pre);

  ast_next = pcq_ast_traverse_next(&trav);

  while(ast_next != NULL) {
    printf("Tag: %s; Contents: %s\n",
      ast_next->tag,
      ast_next->contents);
    ast_next = pcq_ast_traverse_next(&trav);
  }

  pcq_ast_traverse_free(&trav);

  printf("Post order tree traversal.\n");

  trav = pcq_ast_traverse_start(ast, pcq_ast_trav_order_post);

  ast_next = pcq_ast_traverse_next(&trav);

  while(ast_next != NULL) {
    printf("Tag: %s; Contents: %s\n",
      ast_next->tag,
      ast_next->contents);
    ast_next = pcq_ast_traverse_next(&trav);
  }

  pcq_ast_traverse_free(&trav);

  printf("Partial traversal.\n");

  trav = pcq_ast_traverse_start(ast, pcq_ast_trav_order_post);

  ast_next = pcq_ast_traverse_next(&trav);

  for(i=0; i<2 && ast_next != NULL; i++) {
    printf("Tag: %s; Contents: %s\n",
      ast_next->tag,
      ast_next->contents);
    ast_next = pcq_ast_traverse_next(&trav);
  }

  pcq_ast_traverse_free(&trav);

  /* Clean up and return */
  pcq_cleanup(3, Node, Leaf, Input);
  pcq_ast_delete(ast);

  return EXIT_SUCCESS;
}
