package com.example.movei;

import android.app.Fragment;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.design.widget.TabLayout;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.SimpleAdapter;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class list_info_nurse extends Fragment {
    private Context main;
    private TabLayout mTabLayout;
    private ViewPager mViewPager;
    private LayoutInflater mInflater;
    private List<String> mTitleList = new ArrayList<String>();//页卡标题集合
    private View view1, view2, view3;//页卡视图
    private List<View> mViewList = new ArrayList<>();//页卡视图集合

    //适配器
    private String[] names = new String[]{"hushi1", "hushi2", "曹神","B神", "基神", "曹神","B神", "基神", "曹神"};
    private String[] says = new String[]{"无形被黑，最为致命", "大神好厉害~", "我将带头~","无形被黑，最为致命", "大神好厉害~", "我将带头~","无形被黑，最为致命", "大神好厉害~", "我将带头~"};


    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.charts_fragment, container, false);
        mViewPager = (ViewPager) view.findViewById(R.id.vp_view);
        mTabLayout = view.findViewById(R.id.tabs);
        mInflater = LayoutInflater.from(main);


        view1 = mInflater.inflate(R.layout.layout_hello, null);

//        自己的适配器
        List<Map<String, Object>> listitem = new ArrayList<Map<String, Object>>();
        for(int i = 0;i<names.length;i++){
            Map<String, Object> showitem = new HashMap<String, Object>();
            showitem.put("name", names[i]);
            showitem.put("says", says[i]);
            listitem.add(showitem);
        }
        SimpleAdapter myAdapter = new SimpleAdapter(this.getActivity().getApplicationContext(),listitem, R.layout.touxiang, new String[]{"touxiang", "name", "says"}, new int[]{R.id.zhongjie, R.id.name, R.id.tel});
        ListView listView = (ListView) view1.findViewById(R.id.list_test);
        listView.setAdapter(myAdapter);

//        设置监听器
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener(){

            @Override
            public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
                                    long arg3) {
                // TODO Auto-generated method stub
                Intent intent = new Intent(getActivity().getApplicationContext(),info_nurse.class);
                startActivity(intent);
            }

        }) ;

        view2 = mInflater.inflate(R.layout.layout_hello, null);
//        自己的适配器
        List<Map<String, Object>> listitem_view2 = new ArrayList<Map<String, Object>>();
        for(int i = 0;i<names.length;i++){
            Map<String, Object> showitem = new HashMap<String, Object>();
            showitem.put("name", names[i]);
            showitem.put("says", says[i]);
            listitem.add(showitem);
        }
        SimpleAdapter myAdapter_view2 = new SimpleAdapter(this.getActivity().getApplicationContext(),listitem, R.layout.touxiang, new String[]{"touxiang", "name", "says"}, new int[]{R.id.zhongjie, R.id.name, R.id.tel});
        ListView listView_view2 = (ListView) view2.findViewById(R.id.list_test);
        listView_view2.setAdapter(myAdapter_view2);

//        设置监听器
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener(){

            @Override
            public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
                                    long arg3) {
                // TODO Auto-generated method stub
                Intent intent = new Intent(getActivity().getApplicationContext(),info_nurse.class);
                startActivity(intent);
            }

        }) ;

        //添加页卡视图
        mViewList.add(view1);
        mViewList.add(view2);

        //添加页卡标题
        mTitleList.add("值班");
        mTitleList.add("休息");


        //添加tab选项卡，默认第一个选中
        mTabLayout.addTab(mTabLayout.newTab().setText(mTitleList.get(0)), true);

        mTabLayout.addTab(mTabLayout.newTab().setText(mTitleList.get(1)));


        list_info_nurse.MyPagerAdapter mAdapter = new list_info_nurse.MyPagerAdapter(mViewList);
        //给ViewPager设置适配器
        mViewPager.setAdapter(mAdapter);
        //将TabLayout和ViewPager关联起来
        mTabLayout.setupWithViewPager(mViewPager);
        //给Tabs设置适配器
        mTabLayout.setTabsFromPagerAdapter(mAdapter);

        return view;
    }

    public void onAttach(Context context) {
        super.onAttach(context);

        try {
            main = (Context) context;
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    class MyPagerAdapter extends PagerAdapter {
        private List<View> mViewList;

        public MyPagerAdapter(List<View> mViewList) {
            this.mViewList = mViewList;
        }

        @Override
        public int getCount() {
            return mViewList.size();//页卡数
        }

        @Override
        public boolean isViewFromObject(View view, Object object) {
            return view == object;//官方推荐写法
        }

        @Override
        public Object instantiateItem(ViewGroup container, int position) {
            container.addView(mViewList.get(position));//添加页卡
            return mViewList.get(position);
        }

        @Override
        public void destroyItem(ViewGroup container, int position, Object object) {
            container.removeView(mViewList.get(position));//删除页卡
        }

        @Override
        public CharSequence getPageTitle(int position) {
            return mTitleList.get(position);//页卡标题
        }
    }
}
