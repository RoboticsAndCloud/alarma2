package com.madsen.peter.alarma2;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

import com.madsen.peter.alarma2.bt.Connection;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

public class MainActivity extends AppCompatActivity {
    final Connection conn = new Connection("ALARMA2");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ((Button)findViewById(R.id.btn_on)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                action_on();
            }
        });

        ((Button)findViewById(R.id.btn_off)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                action_on();
            }
        });

        ((Button)findViewById(R.id.btn_connect)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                do_connect();
            }
        });
    }

    @Override
    public void onDestroy()
    {
        try {
            conn.disconnect();
        } catch (Exception ignored) {}
        super.onDestroy();
    }

    public void action_on() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    conn.getStatus();
                } catch (Exception ex) {
                } finally {
                }
            }
        }).start();
    }

    public void action_off() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    conn.getStatus();
                } catch (Exception ex) {
                } finally {
                }
            }
        }).start();
//        Toast.makeText(getApplicationContext(), "Turned off", Toast.LENGTH_LONG).show();
    }

    public void do_connect() {
        final EditText log = (EditText)findViewById(R.id.log);

        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    conn.connect();
                } catch (Exception ex) {
                } finally {
                }
            }
        }).start();
    }
}
