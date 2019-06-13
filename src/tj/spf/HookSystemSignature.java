package tj.spf;

import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;
 
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.Signature;
import android.os.Build;
import android.os.Process;
import android.util.Log;
 
/**
 * 替换系统签名
 * 
 * @author sp00f
 * 
 */
public class HookSystemSignature {
 
	private static String tag = "HookSystemSignature";
 
	// 用静态类的原因是防止被gc回收掉
	private static PackageInfo originPkgInfo = null;
	private static PackageInfo newPkgInfo = null;
	private static String signature = "自己指定";
	private Context context;
 
	private static Map<Integer, String> flagsMap = new HashMap<Integer, String>();
 
	static {
		flagsMap.put(1, "GET_ACTIVITIES");
		flagsMap.put(2, "GET_RECEIVERS");
		flagsMap.put(4, "GET_SERVICES");
		flagsMap.put(8, "GET_PROVIDERS");
		flagsMap.put(16, "GET_INSTRUMENTATION");
		flagsMap.put(32, "GET_INTENT_FILTERS");
		flagsMap.put(64, "GET_SIGNATURES");
		flagsMap.put(128, "GET_META_DATA");
		flagsMap.put(256, "GET_GIDS");
		flagsMap.put(512, "GET_DISABLED_COMPONENTS");
		flagsMap.put(1024, "GET_SHARED_LIBRARY_FILES");
		flagsMap.put(2048, "GET_URI_PERMISSION_PATTERNS");
		flagsMap.put(4096, "GET_PERMISSIONS");
		flagsMap.put(8192, "GET_UNINSTALLED_PACKAGES");
		flagsMap.put(16384, "GET_CONFIGURATIONS");
		flagsMap.put(32768, "GET_DISABLED_UNTIL_USED_COMPONENTS");
		flagsMap.put(65536, "MATCH_DEFAULT_ONLY");
	}
 
	public HookSystemSignature(Context context) {
		this.context = context;
 
		if (!init()) {
			throw new RuntimeException("init failed");
		}
	}
 
	public boolean init() {
		try {
			PackageManager pm = context.getPackageManager();
			PackageInfo packageInfo = pm.getPackageInfo(context.getPackageName(), PackageManager.GET_SIGNATURES);
 
			originPkgInfo = packageInfo;
			Signature[] signs = originPkgInfo.signatures;
			Signature sign = signs[0];
			Log.i(tag, "[+] origin signature = " + sign.toCharsString());
 
			return true;
		} catch (Exception e) {
			e.printStackTrace();
		}
 
		return false;
	}
 
	public PackageInfo getNewPackageInfo(String packageName, int flags) throws NameNotFoundException {
 
		Log.e(tag, "[*] GetNewPkgInfo >> packageName = " + packageName + ", flags = " + Integer.toHexString(flags) + " | " + flagsMap.get(flags) + ", pid = "
				+ Process.myPid());
 
		if (flags == PackageManager.GET_SIGNATURES) {
			Log.e(tag, "##Hook_Package_Info_SIG_CALL_STACK##", new Throwable());
		}
		
		 else {
		 Log.e(tag, "##Hook_Package_Info_CALL_STACK##", new Throwable());
		 }
 
		if (newPkgInfo == null) {
			newPkgInfo = originPkgInfo;
 
			Signature[] signs = originPkgInfo.signatures;
			Signature sign = signs[0];
			Log.i(tag, "[+] old signature = " + sign.toCharsString());
 
			Signature risg = new Signature(signature);
			Signature[] psig = new Signature[] { risg };
 
			if (newPkgInfo.signatures.length > 0) {
 
				for (int i = 0; i < newPkgInfo.signatures.length; i++) {
					newPkgInfo.signatures = psig;
 
					Signature[] signs1 = newPkgInfo.signatures;
					Signature sign1 = signs1[0];
					Log.i(tag, "[+] new signature = " + sign1.toCharsString());
				}
			}
 
		}
 
		return newPkgInfo;
	}
 
	/**
	 * 替换系统签名
	 * 
	 * @return 成功返回true
	 */
	public boolean replaceSystemSignature() {
		try {
			Log.i(tag, "[*] hook system signature");
			PackageManager pm = context.getPackageManager();
			Log.i(tag, "[*] android.content.Context method getPackageManager");
 
			Class<?> pmClazz = pm.getClass();
			Method srcMethod = pmClazz.getMethod("getPackageInfo", new Class[] { String.class, int.class });
 
			Class<?> thiz = this.getClass();
			Method dstMethod = thiz.getMethod("getNewPackageInfo", new Class[] { String.class, int.class });
 
			MethodHook hook = new MethodHook();
			int apilevel = Build.VERSION.SDK_INT;
			hook.setApiLevel(apilevel);
			hook.hook(srcMethod, dstMethod);
			Log.i(tag, "[*] replace system signature success");
 
			return true;
 
		} catch (Exception e) {
			e.printStackTrace();
		}
 
		return false;
	}
 
}