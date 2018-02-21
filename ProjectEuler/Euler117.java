/* https://projecteuler.net/problem=117 */
package projectEuler;

import java.util.*;

public class Euler117 {
	public static void main(String[] args) {
		System.out.println(blockSum(50));
	}
	
	public static long blockSum(int sum){
		return blockSum2(1,0,0,0,1,1,sum);
	}

	public static long blockSum2(long one,long two,long three,long four,long count,int pos,int sum){
		if(pos>=sum)
			return count;
		//System.out.println(pos+" "+four+"  "+count);
		return blockSum2(count,one,two,three,count*2-four,++pos,sum);
		
	}
	
}
