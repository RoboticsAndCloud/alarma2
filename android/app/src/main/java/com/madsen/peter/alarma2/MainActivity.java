package com.madsen.peter.alarma2;

import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.Toast;

import com.madsen.peter.alarma2.bt.BTUtils;
import com.madsen.peter.alarma2.bt.Connection;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

public class MainActivity extends AppCompatActivity {
    final Connection conn = new Connection();

    private static final String PREFS_NAME = "MyPrefsFile";
    private static final String CFG_BT_DEVICE = "bt_device";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ((Button) findViewById(R.id.btn_on)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                action_on();
            }
        });

        ((Button) findViewById(R.id.btn_off)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                action_on();
            }
        });

        Button btn_select_bt = findViewById(R.id.btn_bt_device);
        btn_select_bt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                action_select_bt();
            }
        });

        // Restore preferences
        SharedPreferences settings = getSharedPreferences(PREFS_NAME, 0);
        String bt_device = settings.getString(CFG_BT_DEVICE, "...");
        btn_select_bt.setText(bt_device);
        Log.d("yyy", bt_device);
    }

    @Override
    protected void onStart() {
        super.onStart();

        final Button btn_select_bt = (Button) findViewById(R.id.btn_bt_device);
        if (btn_select_bt.getText() == "...")
            return;

        do_connect(btn_select_bt.getText().toString());
    }

    private void do_connect(final String device) {
        final EditText log = (EditText) findViewById(R.id.log);
        log.setText("Connecting to " + device + "...");

        final Button btn_on = (Button) findViewById(R.id.btn_on);
        final Button btn_off = (Button) findViewById(R.id.btn_off);

        btn_on.setEnabled(false);
        btn_off.setEnabled(false);

        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    conn.disconnect();
                    conn.connect(device);
                    MainActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            log.setText("Connected to " + device);
                            btn_on.setEnabled(true);
                            btn_off.setEnabled(true);
                        }
                    });
                } catch (final Exception ex) {
                    MainActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            log.setText("Error" + ex.getMessage());
                        }
                    });
                } finally {
                }
            }
        }).start();
    }

    @Override
    public void onStop() {
        super.onStop();

        final Button btn_on = (Button) findViewById(R.id.btn_on);
        final Button btn_off = (Button) findViewById(R.id.btn_off);

        btn_on.setEnabled(false);
        btn_off.setEnabled(false);

        try {
            conn.disconnect();
        } catch (Exception ignored) {
        }


        // We need an Editor object to make preference changes.
        // All objects are from android.context.Context

        final Button btn_select_bt = (Button) findViewById(R.id.btn_bt_device);

        SharedPreferences settings = getSharedPreferences(PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(CFG_BT_DEVICE, btn_select_bt.getText().toString());
        Log.d("xxx", btn_select_bt.getText().toString());
        editor.commit();
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

    private void action_select_bt() {
        if (!BTUtils.isEnabled()) {
            Intent enableBluetooth = new Intent(
                    BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBluetooth, 0);
            return;
        }

        final CharSequence[] choiceList = BTUtils.get_devices(this);
        if (choiceList == null) {
            return;
        }
//        Log.d(LOG_NAME, "###### " + choiceList.length);

        final Button btn_select_bt = (Button) findViewById(R.id.btn_bt_device);

        AlertDialog.Builder alert = new AlertDialog.Builder(this);
        alert.setTitle("BT");
        alert.setItems(choiceList, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                btn_select_bt.setText(choiceList[which]);
                do_connect(choiceList[which].toString());
            }
        });

        alert.show();
    }
}
