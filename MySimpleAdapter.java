package com.example.movei;


import android.content.Context;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.SimpleAdapter;
import android.widget.Toast;

import java.util.List;
import java.util.Map;

public class MySimpleAdapter extends SimpleAdapter {

    Context context ;
    public MySimpleAdapter(Context context,
                           List<? extends Map<String, ?>> data, int resource, String[] from,
                           int[] to) {
        super(context, data, resource, from, to);
        this.context = context ;
        // TODO Auto-generated constructor stub
    }

    /* (non-Javadoc)
     * @see android.widget.SimpleAdapter#getView(int, android.view.View, android.view.ViewGroup)
     */
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        // TODO Auto-generated method stub
        View view= super.getView(position, convertView, parent);
        LinearLayout btn=(LinearLayout) view.findViewById(R.id.list_info);
        btn.setTag(position);
        btn.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                Toast.makeText(getApplicationContext(), "点击的是ImageButton"+v.getTag(), Toast.LENGTH_LONG).show();
            }
        });
        return view;
    }

    protected Context getApplicationContext() {
        // TODO Auto-generated method stub
        return context;
    }


}