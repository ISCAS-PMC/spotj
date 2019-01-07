package spotj;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

public class SpotJ {
	
	private SpotLibrary spot;
	public static final String INCLUDED = "Included.";
	public static final String NOT_INCLUDED = "Not included: ";
	public static final String CYCLE = "cycle";
	
	public SpotJ() {
		this.spot = Native.load("spotj", SpotLibrary.class);
	}
	
	public static void main(String[] args) {
		final SpotJ spot = new SpotJ();
		String result = spot.is_included("/home/liyong/git/spot/A.hoa", "/home/liyong/git/spot/B.hoa");
		System.out.printf("%s\n", result);
	}
	
	public SpotLibrary getSpot() {
		return spot;
	}
	
	public String is_included(String a, String b) {
		Pointer rt = spot.new_ce();
		spot.is_included(rt, a, b);
		StringBuilder sb = new StringBuilder();
		for(int i = 0; i < spot.size_ce(rt); i ++) {
			sb.append("" + spot.get_ce_char(rt, i));
		}
		spot.free_ce(rt);
		return sb.toString();
	}

	public String getIncluded() {
		return INCLUDED;
	}

	public String getNotincluded() {
		return NOT_INCLUDED;
	}

	public String cycle() {
		return CYCLE;
	}
	
	/**
	 * spot library
	 * 
	 * */
	public static interface SpotLibrary extends Library
	{
		/**
		 * structure to put counterexample
		 * */
		Pointer new_ce();
		/**
		 * check whether aut a is included in automaton b
		 * */
	    void is_included(Pointer rt, String a, String b);
	    /**
		 * size of counterexample
		 * */
	    int size_ce(Pointer rt);
	    /**
		 * get character at i of counterexample
		 * */
	    char get_ce_char(Pointer rt, int i);
	    
	    /**
	     * clean the memory
	     * **/
	    void free_ce(Pointer rt);
	    
	    // interface for manipulating automata in spot
	    /*
	    void aut_is_included(Pointer rt, Pointer aut_a, Pointer aut_b);

	    // create an alphabet
	    Pointer aut_new_dict();

	    // create an automaton
	    Pointer aut_new_twa(Pointer dict);

	    // register a proposition
	    // cause fatal error in runtime
	    int aut_add_prop(Pointer aut, String ap);

	    // set acceptance acc = "Inf(0)"
	    void aut_set_acceptance(Pointer aut, int numOfIndex, String acc);

	    // state number
	    void aut_new_states_num(Pointer aut, int num);

	    // initial state
	    void aut_set_init_state(Pointer aut, int init);

	    // neg indicates whether add the negation of ap
	    void aut_new_edge(Pointer aut, int s, int t, boolean[] ap, int size_ap);

	    // add a true label on the transition
	    void aut_new_true_edge(Pointer aut, int s, int t);

	    // add a marked label on transition
	    void aut_new_edge_label(Pointer aut, int s, int t, boolean[] ap, int size_ap, int[] marks, int size_marks);

	    // add a makred true label
	    void aut_new_true_edge_label(Pointer aut, int s, int t, int[] marks, int size_marks);

	    // -------------- interface for visiting an automaton
	    boolean aut_is_empty(Pointer aut);
		*/
	}

}
