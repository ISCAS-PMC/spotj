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

typedef struct result{
	string ce;
} result;

typedef result* result_ptr;

const char* str_included = "Included.";
const char* str_not_included = "Not included: ";

static string
is_included(spot::twa_graph_ptr aut_a, spot::twa_graph_ptr aut_b) 
{
	auto complement_aut_b = spot::dualize(aut_b);
	spot::twa_word_ptr word = aut_a->intersecting_word(complement_aut_b);
	std::stringstream ss;
	if(word != nullptr) {
		ss << str_not_included;
		//spot::twa_graph_ptr result = word->as_automaton();
		word->simplify();
		ss << (*word);
		 // Print the resulting automaton.
		//print_hoa(std::cout, result);
    }else {
		ss << str_included;
	}
	return ss.str();
}

extern "C" {
    
    
__attribute__ ((visibility("default")))
result*
new_ce() 
{
	return new result;
}

__attribute__ ((visibility("default")))
int 
size_ce(result_ptr rt)
{
	return (rt->ce).length();
}

__attribute__ ((visibility("default")))
char
get_ce_char(result_ptr rt, int i) 
{
	return (rt->ce)[i];
}

__attribute__ ((visibility("default")))
void 
free_ce(result_ptr rt) 
{
	if(rt != nullptr){
		delete rt;
	}
}

__attribute__ ((visibility("default")))
void
is_included(result_ptr rt, const char* file_a, const char* file_b) 
{    
	spot::bdd_dict_ptr dict = spot::make_bdd_dict();
	auto aut_a = read_automaton(file_a, dict);
	auto aut_b = ensure_deterministic(read_automaton(file_b, dict), true);
    string str = is_included(aut_a, aut_b);
	rt->ce = str;
}

__attribute__ ((visibility("default")))
const char*
cycle()
{
	return "cycle";
}

__attribute__ ((visibility("default")))
const char*
not_included()
{
	return str_not_included;
}

__attribute__ ((visibility("default")))
const char*
included()
{
	return str_included;
}

// --------------------- interface for constructing nba

__attribute__ ((visibility("default")))
void
aut_is_included(result_ptr rt, spot::twa_graph_ptr aut_a, spot::twa_graph_ptr aut_b) 
{
	aut_b = ensure_deterministic(aut_b, true);
    string str = is_included(aut_a, aut_b);
	rt->ce = str;
}

// create an alphabet
__attribute__ ((visibility("default")))
spot::bdd_dict_ptr
aut_new_dict()
{
	return spot::make_bdd_dict();
}

// create an automaton
__attribute__ ((visibility("default")))
spot::twa_graph_ptr
aut_new_twa(spot::bdd_dict_ptr dict)
{
	return make_twa_graph(dict);
}

// register a proposition
__attribute__ ((visibility("default")))
int
aut_add_prop(spot::twa_graph_ptr aut, const char* ap)
{
	return aut->register_ap(ap);
}

__attribute__ ((visibility("default")))
void
aut_set_acceptance(spot::twa_graph_ptr aut, int numOfIndex, const char* acc)
{
	aut->set_acceptance(numOfIndex, acc);
}

__attribute__ ((visibility("default")))
void 
aut_new_states_num(spot::twa_graph_ptr aut, int num)
{
	aut->new_states(num);
}

__attribute__ ((visibility("default")))
void
aut_set_init_state(spot::twa_graph_ptr aut, int init)
{
	aut->set_init_state(init);
}

__attribute__ ((visibility("default")))
void
aut_new_edge(spot::twa_graph_ptr aut, int s, int t, const bool* ap, int size_ap)
{
	bdd label = bddfalse;
	for(int i = 0; i < size_ap; i ++) 
	{
		bdd lit = ap[i] ? bdd_ithvar(i) : ! bdd_ithvar(i);
		if(i == 0) 
		{
			label = lit;
		}else 
		{
			label = label & lit;
		}
	}
	aut->new_edge(s, t, label);
}

__attribute__ ((visibility("default")))
void
aut_new_true_edge(spot::twa_graph_ptr aut, int s, int t)
{
	aut->new_edge(s, t, bddtrue);
}

__attribute__ ((visibility("default")))
void
aut_new_edge_label(spot::twa_graph_ptr aut, int s, int t, const bool* ap, int size_ap, const int* marks, int size_marks)
{
	// first convert it to acc_cond::mark_t
	spot::acc_cond::mark_t m = {};
    for(int i = 0; i < size_marks; i ++) 
    {
		m.set(marks[i]);
	}
	bdd label = bddfalse;
	for(int i = 0; i < size_ap; i ++) 
	{
		bdd lit = ap[i] ? bdd_ithvar(i) : ! bdd_ithvar(i);
		if(i == 0) 
		{
			label = lit;
		}else 
		{
			label = label & lit;
		}
	}
	aut->new_edge(s, t, label, m);
}

__attribute__ ((visibility("default")))
void
aut_new_true_edge_label(spot::twa_graph_ptr aut, int s, int t, const int* marks, int size_marks)
{
	spot::acc_cond::mark_t m = {};
    for(int i = 0; i < size_marks; i ++) 
    {
		m.set(marks[i]);
	}
	aut->new_edge(s, t, bddtrue, m);
}

// -------------- interface for visiting an automaton
__attribute__ ((visibility("default")))
bool 
aut_is_empty(spot::twa_graph_ptr aut)
{
	return aut->is_empty();
}


// end of extern
}
	

int main(int argc, char** args) 
{
	if(argc != 3) {
		cerr<<"please input two automata, current number of arguments are " << argc << endl;
		return -1; 
	}
	
	spot::bdd_dict_ptr dict = spot::make_bdd_dict();
	auto autB = ensure_deterministic(read_automaton(args[2], dict), true);
    auto autA = read_automaton(args[1], dict);
    string ce = is_included(autA, autB);
    cout << ce.c_str() << "\n";
	/*
	cout << "-1";
	// test automaton construction
	spot::bdd_dict_ptr dd = aut_new_dict();
	cout << "0";
	
	// This creates an empty automaton that we have yet to fill.
	spot::twa_graph_ptr aut = aut_new_twa(dd);
    cout << "1";
    
	int a = aut_add_prop(aut, "a");
	cout << "2 -> " << a << endl;
	
	// Set the acceptance condition of the automaton to co-Büchi
	aut_set_acceptance(aut, 1, "Inf(0)");
	cout << "3";
	
	// States are numbered from 0.
	aut_new_states_num(aut, 3);
	cout << "4";
	
  // The default initial state is 0, but it is always better to
  // specify it explicitely.
	aut_set_init_state(aut, 0U);
	cout << "5";
	
  // new_edge() takes 3 mandatory parameters: source state,
  // destination state, and label.  A last optional parameter can be
  // used to specify membership to acceptance sets.
  //
  // new_univ_edge() is similar, but the destination is a set of
  // states.
	bool* mark = new bool[1];
	mark[0] = true;
    cout << "6";
	aut_new_edge(aut, 0, 0, mark, 1);
	cout << "7";
	int* m = new int[1];
	m[0] = 0;
	mark[0] = false;
	aut_new_edge_label(aut, 1, 1, mark, 1, m, 1);
		mark[0] = true;

	cout << "8";
	aut_new_edge(aut, 1, 2, mark, 1);
	cout << "9";
	
	aut_new_edge(aut, 2, 2, mark, 1);
	cout << "10";
	mark[0] = false;
	aut_new_edge(aut, 2, 2, mark, 1);
	cout << "11" << endl;
	// Print the resulting automaton.
	print_hoa(std::cout, aut);
	*/
	return 0;
}
