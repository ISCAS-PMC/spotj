package spotj;

import com.sun.jna.Library;

public interface SpotLibrary extends Library
{
	/**
	 * check whether aut a is included in automaton b
	 * */
    String is_included(String a, String b);
    /**
     * the indication in the output for inclusion
     * */
    String included();
    /**
     * the indication in the output for noninclusion
     * */
    String not_included();
    /**
     * the indication for the start of cycle word
     * */
    String cycle();
}
