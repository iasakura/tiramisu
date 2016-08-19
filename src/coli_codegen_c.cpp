#include <isl/aff.h>
#include <isl/set.h>
#include <isl/map.h>
#include <isl/union_map.h>
#include <isl/union_set.h>
#include <isl/ast_build.h>
#include <isl/schedule.h>
#include <isl/schedule_node.h>

#include <coli/debug.h>
#include <coli/core.h>

#include <string>


void coli::library::gen_c_code()
{
	coli::str_dump("\n\n");
	coli::str_dump("\nC like code:\n");
	isl_printer *p;
	p = isl_printer_to_file(this->get_ctx(), stdout);
	p = isl_printer_set_output_format(p, ISL_FORMAT_C);
	p = isl_printer_print_ast_node(p, this->get_isl_ast());
	isl_printer_free(p);
	coli::str_dump("\n\n");
}