package com.example.movei;




import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Toast;

import com.xys.libzxing.zxing.activity.CaptureActivity;


public class MainActivity extends AppCompatActivity {
    private String[] items = {"首页", "日志", "设置"};
    private doctor doctor = new doctor();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_main);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);

        setSupportActionBar(toolbar);
        getSupportActionBar().setHomeButtonEnabled(true);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        final DrawerLayout mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle mToggle = new ActionBarDrawerToggle(this, mDrawerLayout, toolbar, R.string.open, R.string.close);
        mToggle.syncState();

        mDrawerLayout.addDrawerListener(mToggle);

        final FragmentManager fragmentManager = getFragmentManager();
//        fragmentManager.beginTransaction().add(R.id.ll_content, new doctor()).commit();
        fragmentManager.beginTransaction().add(R.id.ll_content, doctor, "apple")
//                .add(R.id.ll_content, videoFragment, "video")
//                .add(R.id.ll_content, weatherFragment, "weather")
//                .add(R.id.ll_content, newsFragment, "news")
                .show(doctor)
//                .hide(newsFragment)
//                .hide(videoFragment)
//                .hide(weatherFragment)
                .commit();


        ListView lv_drawer = (ListView) findViewById(R.id.lv_drawer);
        ArrayAdapter mAdapter = new ArrayAdapter(this, android.R.layout.simple_list_item_1, items);
        lv_drawer.setAdapter(mAdapter);
        final LinearLayout ll_drawer = (LinearLayout) findViewById(R.id.ll_drawer);
        final Toolbar finalToolbar = toolbar;
        lv_drawer.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                switchFragment(position);
            }

            public void switchFragment(int fragmentId) {
                mDrawerLayout.closeDrawer(ll_drawer);
                int currentFragmentId = 5;
                if(currentFragmentId == fragmentId)
                    return;
                currentFragmentId = fragmentId;
                FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
                switch (fragmentId)
                {
                    case 0:
                        fragmentTransaction.replace(R.id.ll_content, new doctor());
                        finalToolbar.setTitle("首页");
                        break;
                    case 1:
                        fragmentTransaction.replace(R.id.ll_content, new doctor());
                        finalToolbar.setTitle("日志");
                        break;
                    case 2:
                        fragmentTransaction.replace(R.id.ll_content, new doctor());
                        finalToolbar.setTitle("设置");
                        break;
                }
                fragmentTransaction.commit();
            }
        });

    }
    protected void onActivityResult(int requestCode,int resultCode,Intent data) {
        super.onActivityResult(requestCode,resultCode,data);
        if(resultCode == RESULT_OK){
            Bundle bundle = data.getExtras();
            String result = bundle.getString("result");
            Toast.makeText(this, result, Toast.LENGTH_SHORT).show();
        }
    }
    public boolean onCreateOptionsMenu(Menu menu) {
        super.onCreateOptionsMenu(menu);
        getMenuInflater().inflate(R.menu.toolbar_view, menu);
        return true;
    }
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_location:
                Toast.makeText(this, "分享", Toast.LENGTH_SHORT).show();
                break;
            case R.id.action_share:
                Toast.makeText(this, "分享", Toast.LENGTH_SHORT).show();
                break;
            case R.id.scan:
                Intent intent_search = new Intent(getApplicationContext(),info_search.class);
                startActivity(intent_search);
                break;
            case R.id.search:
                startActivityForResult(new Intent(MainActivity.this,CaptureActivity.class),0);
                break;
        }
        return true;
    }
}
