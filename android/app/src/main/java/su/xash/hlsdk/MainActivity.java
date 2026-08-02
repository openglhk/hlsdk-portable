package su.xash.hlsdk;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        StringBuilder cmdLine = new StringBuilder();
        cmdLine.append("-dev 3 ")
			   .append("-log ")
               .append("-game czeror ")
               .append("+map cz_train01 ");
        String pkg = "su.xash.engine.test";
        try {
            getPackageManager().getPackageInfo(pkg, 0);
        } catch(android.content.pm.PackageManager.NameNotFoundException e) {
            try {
                pkg = "su.xash.engine";
                getPackageManager().getPackageInfo(pkg, 0);
            } catch(android.content.pm.PackageManager.NameNotFoundException ex) {
                startActivity(new android.content.Intent(android.content.Intent.ACTION_VIEW,
                              android.net.Uri.parse("https://github.com/FWGS/xash3d-fwgs/releases/tag/continuous"))
                              .setFlags(android.content.Intent.FLAG_ACTIVITY_NEW_TASK | android.content.Intent.FLAG_ACTIVITY_CLEAR_TASK));
                finish();
                return;
            }
        }
        android.content.Intent intent = new android.content.Intent();
        intent.setComponent(new android.content.ComponentName(pkg, "su.xash.engine.XashActivity"));
        intent.setFlags(android.content.Intent.FLAG_ACTIVITY_NEW_TASK | android.content.Intent.FLAG_ACTIVITY_CLEAR_TASK);
        intent.putExtra("gamedir", "czeror");
        intent.putExtra("gamelibdir", getApplicationInfo().nativeLibraryDir);
        intent.putExtra("argv", cmdLine.toString().trim()); 
        intent.putExtra("package", getPackageName());
        
        startActivity(intent);
        finish();
    }
}
