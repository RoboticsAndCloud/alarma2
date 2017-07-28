package com.madsen.peter.alarma2;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

public class MainActivity extends AppCompatActivity {
    BluetoothAdapter mBA;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ((Button)findViewById(R.id.btn_on)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                on(view);
            }
        });

        ((Button)findViewById(R.id.btn_off)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                off(view);
            }
        });

        ((Button)findViewById(R.id.btn_list)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                list(view);
            }
        });


        mBA = BluetoothAdapter.getDefaultAdapter();
    }

    public void on(View v) {
        if (!mBA.isEnabled()) {
            Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(turnOn, 0);
            Toast.makeText(getApplicationContext(), "Turned on", Toast.LENGTH_LONG).show();
        } else {
            Toast.makeText(getApplicationContext(), "Already on", Toast.LENGTH_LONG).show();
        }
    }

    public void off(View v) {
        mBA.disable();
        Toast.makeText(getApplicationContext(), "Turned off", Toast.LENGTH_LONG).show();
    }

    public void list(View v){
        Set<BluetoothDevice> pairedDevices = mBA.getBondedDevices();

        List list = new ArrayList();

        for(BluetoothDevice bt : pairedDevices) list.add(bt.getName());
        Toast.makeText(getApplicationContext(), "Showing Paired Devices",Toast.LENGTH_SHORT).show();

        final ArrayAdapter adapter = new  ArrayAdapter(this,android.R.layout.simple_list_item_1, list);

        ListView lv = (ListView)findViewById(R.id.lv);
        lv.setAdapter(adapter);
    }
}
