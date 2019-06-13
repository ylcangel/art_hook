package tj.spf;

import java.lang.reflect.Method;
 
import android.util.Log;
 
/**
 * hook功能类，你应该调用这里的方法，和初始化这个类，而不是ArtMethod
 * @author sp00f
 * @version   0.1
 * 
 */
public class MethodHook {
	
	private String tag = "MethodHook";
	
	private Method backedMethod = null;
 
	public MethodHook() {
		try {
			backedMethod = this.getClass().getDeclaredMethod("back", new Class<?>[]{});
			backedMethod.invoke(this, new Object[]{});
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	//这个方法是为了占个坑
	public void back() {
		Log.i(tag, "[*] In order to occupy the pit, so keep this method");
	}
	
	/**
	 * 不带备份源函数的hook
	 * @param src，源函数
	 * @param dst，目标函数
	 * @return，是否成功
	 */
	public boolean hook(Method src, Method dst) {
		return ArtMethod.cloneMethod(src, dst);
	}
	
	/**
	 * 带拷贝的源函数的hook
	 * @param src，源函数
	 * @param dst，目标函数
	 * @return，是否成功
	 */
	public boolean hookAndBack(Method src, Method dst) {
		if (!backedMethod(src)) {
			Log.e(tag, "[-] clone origin method failed");
			return false;
		}
		
		return ArtMethod.cloneMethod(src, dst);
	}
	
	/**
	 * 函数备份
	 * 因为原来backed方法有原始方法对应，克隆后将丢失（这个提前预留的坑）
	 * @param origin，要被备份的方法，克隆后origin方法保持不变，只是backedMethod由原来的(本类中的back())
	 * 					变成了origin方法（同一份拷贝）
	 * @return 成功返回true
	 */
	public boolean backedMethod(Method origin) {
		return ArtMethod.cloneMethod(backedMethod, origin);
	}
	
	public Object callOrgin(Object receiver, Object...args) {
		try {
			return backedMethod.invoke(receiver, args);
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		return null;
	}
	
	/**
	 * 设置api level，用于判断底层art method类型
	 * @param l 对于api level
	 */
	public void setApiLevel(int l) { // int apilevel = Build.VERSION.SDK_INT;
		ArtMethod.setApiLevel(l);
	}
	
	public Method getBackedMethod() {
		return backedMethod;
	}
}
