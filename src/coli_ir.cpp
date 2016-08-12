#include <isl/aff.h>
#include <isl/set.h>
#include <isl/map.h>
#include <isl/union_map.h>
#include <isl/union_set.h>
#include <isl/ast_build.h>
#include <isl/schedule.h>
#include <isl/schedule_node.h>

#include <coli/debug.h>
#include <coli/ir.h>

#include <string>

std::map<std::string, Computation *> computations_list;

// Used for the generation of new variable names.
int id_counter = 0;


isl_schedule *create_schedule_tree(isl_ctx *ctx,
		   isl_union_set *udom,
		   isl_union_map *sched_map)
{
	isl_union_set *scheduled_domain = isl_union_set_apply(udom, sched_map);
	IF_DEBUG2(coli_str_dump("[ir.c] Scheduled domain: "));
	IF_DEBUG2(isl_union_set_dump(scheduled_domain));

	isl_schedule *sched_tree = isl_schedule_from_domain(scheduled_domain);

	return sched_tree;
}

/* Schedule the iteration space.  */
isl_union_set *create_time_space(__isl_take isl_union_set *set, __isl_take isl_union_map *umap)
{
	return isl_union_set_apply(set, umap);
}

isl_ast_node *generate_code(isl_ctx *ctx,
		   isl_schedule *sched_tree)
{
	isl_ast_build *ast = isl_ast_build_alloc(ctx);
 	isl_ast_node *program = isl_ast_build_node_from_schedule(ast, sched_tree);
	isl_ast_build_free(ast);

	return program;
}

void split_string(std::string str, std::string delimiter,
		  std::vector<std::string> &vector)
{
	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delimiter)) != std::string::npos) {
		token = str.substr(0, pos);
		vector.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	token = str.substr(0, pos);
	vector.push_back(token);
}

void isl_space_tokens::Parse(std::string space)
{
	split_string(space, ",", this->dimensions);
}

std::string generate_new_variable_name()
{
	return "c" + std::to_string(id_counter++);
}

// Function related methods

void IRFunction::add_computation_to_body(Computation *cpt)
{
	this->body.push_back(cpt);
}

void IRFunction::add_computation_to_signature(Computation *cpt)
{
	this->signature.push_back(cpt);
}

void IRFunction::dump()
{
	if (DEBUG)
	{
		std::cout << "Function \"" << this->name << "\"" << std::endl;
		std::cout << "Body " << std::endl;

		for (auto cpt : this->body)
		       cpt->dump();

		std::cout << "Signature:" << std::endl;

		for (auto cpt : this->signature)
		       cpt->dump();

		std::cout << std::endl;
	}
}

void IRFunction::dump_ISIR()
{
	if (DEBUG)
	{
		for (auto cpt : this->body)
		       cpt->dump_ISIR();
	}
}

void IRFunction::dump_schedule()
{
	if (DEBUG)
	{
		for (auto cpt : this->body)
		       cpt->dump_schedule();
	}
}


// Program related methods

void IRProgram::tag_parallel_dimension(std::string stmt_name,
				      int par_dim)
{
	if (par_dim >= 0)
		this->parallel_dimensions.insert(
				std::pair<std::string,int>(stmt_name,
							   par_dim));
}

void IRProgram::tag_vector_dimension(std::string stmt_name,
		int vec_dim)
{
	if (vec_dim >= 0)
		this->vector_dimensions.insert(
				std::pair<std::string,int>(stmt_name,
					                   vec_dim));
}

void IRProgram::dump_ISIR()
{
	if (DEBUG)
	{
		coli_str_dump("\nIteration Space IR:\n");
		for (const auto &fct : this->functions)
		       fct->dump_ISIR();
		coli_str_dump("\n");
	}
}

void IRProgram::dump_schedule()
{
	if (DEBUG)
	{
		coli_str_dump("\nSchedule:\n");
		for (const auto &fct : this->functions)
		       fct->dump_schedule();

		std::cout << "Parallel dimensions: ";
		for (auto par_dim: parallel_dimensions)
			std::cout << par_dim.first << "(" << par_dim.second << ") ";

		std::cout << std::endl;

		std::cout << "Vector dimensions: ";
		for (auto vec_dim: vector_dimensions)
			std::cout << vec_dim.first << "(" << vec_dim.second << ") ";

		std::cout<< std::endl << std::endl << std::endl;
	}
}

void IRProgram::dump()
{
	if (DEBUG)
	{
		std::cout << "Program \"" << this->name << "\"" << std::endl
			  <<
			std::endl;

		for (const auto &fct : this->functions)
		       fct->dump();

		std::cout << "Parallel dimensions: ";
		for (auto par_dim: parallel_dimensions)
			std::cout << par_dim.first << "(" << par_dim.second << ") ";

		std::cout << std::endl;

		std::cout << "Vector dimensions: ";
		for (auto vec_dim: vector_dimensions)
			std::cout << vec_dim.first << "(" << vec_dim.second << ") ";

		std::cout<< std::endl << std::endl;
	}
}

void IRProgram::add_function(IRFunction *fct)
{
	this->functions.push_back(fct);
}

isl_union_set * IRProgram::get_iteration_spaces()
{
	isl_union_set *result;
	isl_space *space;

	if (this->functions.empty() == false)
	{
		if(this->functions[0]->body.empty() == false)
			space = isl_set_get_space(this->functions[0]->body[0]->iter_space);
	}
	else
		return NULL;

	result = isl_union_set_empty(isl_space_copy(space));

	for (const auto &fct : this->functions)
		for (const auto &cpt : fct->body)
		{
			isl_set *cpt_iter_space = isl_set_copy(cpt->iter_space);
			result = isl_union_set_union(isl_union_set_from_set(cpt_iter_space), result);
		}

	return result;
}

isl_union_map * IRProgram::get_schedule_map()
{
	isl_union_map *result;
	isl_space *space;

	if (this->functions.empty() == false)
	{
		if(this->functions[0]->body.empty() == false)
			space = isl_map_get_space(this->functions[0]->body[0]->schedule);
	}
	else
		return NULL;

	result = isl_union_map_empty(isl_space_copy(space));

	for (const auto &fct : this->functions)
		for (const auto &cpt : fct->body)
		{
			isl_map *m = isl_map_copy(cpt->schedule);
			result = isl_union_map_union(isl_union_map_from_map(m), result);
		}

	return result;
}


// Halide IR related methods

void halide_IR_dump(Halide::Internal::Stmt s)
{
	if (DEBUG)
	{
		coli_str_dump("\n\n");
		coli_str_dump("\nGenerated Halide Low Level IR:\n");
		Halide::Internal::IRPrinter pr(std::cout);
	    	pr.print(s);
		coli_str_dump("\n\n\n\n");
	}
}
