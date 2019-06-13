package tj.spf;

import java.lang.reflect.Method;
 
import android.util.Log;
/**
 * MethodHook辅助类， native 对应art_method.h 和 art_method.cpp
 * 你最好不要直接调用这个类，hook功能都是通过MethodHook提供的
 * @author sp00f
 * @version   0.1
 */
public class ArtMethod {
	
//	static {
//		try {
//			System.loadLibrary("hk");
//		} catch (Error e) {
//			throw new Error(e);
//		} catch (Exception e1) {
//			throw new RuntimeException(e1);
//		}
//	}
	
	private String tag = ArtMethod.class.getName();
 
	public ArtMethod() {}
 
	////被包围的函数是用来测试ArtMethod是哪种类型的////////////////////
	////根据两个方法的之间宽度判断//////////////////////////////////
	public static native void m0();
	
	public void m1() {
		Log.i("SimpleMethod", "m1");
	}
	
	public void m2() {
		Log.i("SimpleMethod", "m2");
	}
	////被包围的函数是用来测试ArtMethod是哪种类型的////////////////////
	
	/**
	 * 克隆方法用的,同时也是hook的主方法，此方法只完成native层method结构拷贝
	 * 利用此方法，origin对应的native层method结构将失去和java层method结构的联系
	 * @param origin 即将被hook的方法
	 * @param dst 替换原方法的方法
	 * @return 是否成功
	 */
	public static native boolean cloneMethod(Method origin, Method dst);
	
	/**
	 * 设置api level，用于判断底层art method类型
	 * @param l 对于api level
	 */
	public static native void setApiLevel(int l);
	
}