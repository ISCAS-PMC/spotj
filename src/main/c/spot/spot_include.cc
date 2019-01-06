#include <spot/twaalgos/dualize.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/product.hh>
#include <spot/twaalgos/word.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twa/twagraph.hh>
#include <spot/parseaut/public.hh>
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

static spot::twa_graph_ptr
ensure_deterministic(const spot::twa_graph_ptr& aut, bool nonalt = false)
{
  if ((!nonalt || aut->is_existential()) && spot::is_universal(aut))
    return aut;
  spot::postprocessor p;
  p.set_type(spot::postprocessor::Generic);
  p.set_pref(spot::postprocessor::Deterministic);
  p.set_level(spot::postprocessor::Low);
  return p.run(aut);
}

static spot::twa_graph_ptr ensure_tba(spot::twa_graph_ptr aut)
{
  spot::postprocessor p;
  p.set_type(spot::postprocessor::TGBA);
  p.set_pref(spot::postprocessor::Any);
  p.set_level(spot::postprocessor::Low);
  return spot::degeneralize_tba(p.run(aut));

}

spot::automaton_parser_options options;
static spot::twa_graph_ptr
read_automaton(const char* filename, spot::bdd_dict_ptr& dict)
{
  auto p = spot::parse_aut(filename, dict,
                           spot::default_environment::instance(),
                           options);
  if (p->format_errors(std::cerr)) 
  {
	  cerr << "failed to read automaton from " << filename;
	  std::exit(-1);
  }
  if (p->aborted) 
  {
	  cerr << "failed to read automaton from %s (--ABORT-- read)" << filename;
	  std::exit(-1);
  }
  return std::move(p->aut);
}


static spot::twa_graph_ptr
product(spot::twa_graph_ptr left, spot::twa_graph_ptr right)
{
  if ((left->num_sets() + right->num_sets() >
          spot::acc_cond::mark_t::max_accsets()))
    {
      left = ensure_tba(left);
      right = ensure_tba(right);
    }
  return spot::product(left, right);
}

extern "C"
const char*
included()
{
	return "Included.";
}

extern "C" 
const char*
not_included()
{
	return "Not included: ";
}

extern "C" 
const char*
cycle()
{
	return "cycle";
}

static string
is_included(spot::twa_graph_ptr aut_a, spot::twa_graph_ptr aut_b) 
{
	auto complement_aut_b = spot::dualize(aut_b);
	spot::twa_word_ptr word = aut_a->intersecting_word(complement_aut_b);
	std::stringstream ss;
	if(word != nullptr) {
		ss << not_included();
		//spot::twa_graph_ptr result = word->as_automaton();
		word->simplify();
		ss << (*word);
		 // Print the resulting automaton.
		//print_hoa(std::cout, result);
    }else {
		ss << included();
	}
	return ss.str();
}

extern "C" 
const char *
is_included(const char* file_a, const char* file_b) 
{    
	spot::bdd_dict_ptr dict = spot::make_bdd_dict();
	auto aut_a = read_automaton(file_a, dict);
	auto aut_b = ensure_deterministic(read_automaton(file_b, dict), true);
    string str = is_included(aut_a, aut_b);
    char* result = (char*) malloc(str.length());
    strcpy (result, str.c_str());
    return result;
}


int main(int argc, char** args) 
{
	if(argc < 1 || argc > 4) {
		cerr<<"please input two automata: " << argc;
		return -1;
	}
	spot::bdd_dict_ptr dict = spot::make_bdd_dict();
	auto autB = ensure_deterministic(read_automaton(args[2], dict), true);
    auto autA = read_automaton(args[1], dict);
    string ce = is_included(autA, autB);
    cout << ce.c_str() << "\n";
    cout << args[1] << endl;
    cout << args[2] << endl;
    const char* rt = is_included(args[1], args[2]);
    cout << rt << endl;
    cout << included() << endl;
    cout << not_included() << endl;
	return 0;
}
