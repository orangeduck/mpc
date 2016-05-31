#include "../mpc.h"

int main(int argc, char *argv[]) {

  mpc_parser_t *Input  = mpc_new("input");
  mpc_parser_t *Node  = mpc_new("node");
  mpc_parser_t *Leaf  = mpc_new("leaf");
  mpc_ast_t *ast, *tree, *child, *child_sub, *ast_next;
  mpc_ast_trav_t *trav;
  mpc_result_t r;
  int index, lb, i;

  mpca_lang(MPCA_LANG_PREDICTIVE,
        " node : '(' <node> ',' /foo/ ',' <node> ')' | <leaf>;"
        " leaf : /bar/;"
        " input : /^/ <node> /$/;",
        Node, Leaf, Input, NULL);

  if (argc > 1) {

    if (mpc_parse_contents(argv[1], Input, &r)) {
      ast = r.output;
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
      mpc_cleanup(3, Node, Leaf, Input);
      return EXIT_FAILURE;
    }

  } else {

    if (mpc_parse_pipe("<stdin>", stdin, Input, &r)) {
      ast = r.output;
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
      mpc_cleanup(3, Node, Leaf, Input);
      return EXIT_FAILURE;
    }

  }

  /* Get index or child of tree */
  tree = ast->children[1];

  index = mpc_ast_get_index(tree, "node|>");
  child = mpc_ast_get_child(tree, "node|>");

  if(child == NULL) {
    mpc_cleanup(3, Node, Leaf, Input);
    mpc_ast_delete(ast);
    return EXIT_FAILURE;
  }

  printf("Index: %d; Child: \"%s\"\n", index, child->tag);

  /* Get multiple indexes or children of trees */
  index     = mpc_ast_get_index_lb(child, "node|leaf|regex", 0);
  child_sub = mpc_ast_get_child_lb(child, "node|leaf|regex", 0);

  while(index != -1) {
    printf("-- Index: %d; Child: \"%s\"\n", index, child_sub->tag);

    lb = index + 1;
    index     = mpc_ast_get_index_lb(child, "node|leaf|regex", lb);
    child_sub = mpc_ast_get_child_lb(child, "node|leaf|regex", lb);
  }

  /* Traversal */
  printf("Pre order tree traversal.\n");
  trav = mpc_ast_traverse_start(ast, mpc_ast_trav_order_pre);

  ast_next = mpc_ast_traverse_next(&trav);

  while(ast_next != NULL) {
    printf("Tag: %s; Contents: %s\n",
      ast_next->tag,
      ast_next->contents);
    ast_next = mpc_ast_traverse_next(&trav);
  }

  mpc_ast_traverse_free(&trav);

  printf("Post order tree traversal.\n");

  trav = mpc_ast_traverse_start(ast, mpc_ast_trav_order_post);

  ast_next = mpc_ast_traverse_next(&trav);

  while(ast_next != NULL) {
    printf("Tag: %s; Contents: %s\n",
      ast_next->tag,
      ast_next->contents);
    ast_next = mpc_ast_traverse_next(&trav);
  }

  mpc_ast_traverse_free(&trav);

  printf("Partial traversal.\n");

  trav = mpc_ast_traverse_start(ast, mpc_ast_trav_order_post);

  ast_next = mpc_ast_traverse_next(&trav);

  for(i=0; i<2 && ast_next != NULL; i++) {
    printf("Tag: %s; Contents: %s\n",
      ast_next->tag,
      ast_next->contents);
    ast_next = mpc_ast_traverse_next(&trav);
  }

  mpc_ast_traverse_free(&trav);

  /* Clean up and return */
  mpc_cleanup(3, Node, Leaf, Input);
  mpc_ast_delete(ast);

  return EXIT_SUCCESS;
}
