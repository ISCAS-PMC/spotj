package spotj;

import com.sun.jna.Native;

public class SpotJ implements SpotLibrary {
	
	private SpotLibrary spot;
	
	public SpotJ() {
		this.spot = Native.load("spotj", SpotLibrary.class);
	}
	
	public String is_included(String a, String b) {
		return spot.is_included(a, b);
	}

	public String included() {
		return spot.included();
	}

	public String not_included() {
		return spot.not_included();
	}

	public String cycle() {
		return spot.cycle();
	}
	
	public static void main(String[] args) {
		final SpotJ spot = new SpotJ();
		String result = spot.is_included("/home/liyong/git/spot/B.hoa", "/home/liyong/git/spot/B.hoa");
		System.out.printf("%s\n", result);
		System.out.printf("%s\n", spot.included());
		System.out.printf("%s\n", spot.not_included());
	}


}
