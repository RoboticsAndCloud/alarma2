package com.madsen.peter.alarma2.bt;


import android.os.Handler;
import android.util.Log;


public class Connection {
    private static final String LOG_NAME = "ConnectionBT";
    private BT bt = null;

    private Handler mHandler = new Handler();

    public Connection() {
        bt = new BT();
    }


    public void log(final String text) {
        Log.d(LOG_NAME, text);
//        Activity activity = (Activity) getView().getContext();
//        activity.runOnUiThread(new Runnable() {
//            @Override
//            public void run() {
//                final EditText debug = (EditText) getView().findViewById(
//                        R.id.debug);
//                debug.append(text + "\n");
//
//                // scroll to bottom
////				System.err.println(debug.getLineCount());
//                int y = (debug.getLineCount()) * debug.getLineHeight(); // the " - 1" should send it to the TOP of the last line, instead of the bottom of the last line
//                debug.scrollTo(0, y);
//            }
//        });
    }


    public void connect(String bt_device) throws Exception {
        if(bt.isConnected()) {
            log("Already connected");
            return;
        }

        log("Connecting to " + bt_device + "...");
        bt.open(bt_device);
        log("Connected");
    }

    public void getStatus() throws Exception {
        log("Uploading");
        if (!bt.isConnected()) {
            log("Not connected");
            return;
        }

        send(ArduinoCommands.CMD_BEEP);

//        String reply = send(ArduinoCommands.CMD_CONFIG);
//        parse_config_reply(reply);
    }


//    public void upload(final UserDatabase users, Activity activity, final ProgressBar pbar) throws Exception {
//        send(ArduinoCommands.CMD_ERASE_ALL);
//
//        for(int i=0; i<users.size(); ++i) {
//            send(ArduinoCommands.CMD_ADD_USER + users.tags().get(i).tag);
//
//            activity.runOnUiThread(new Runnable() {
//                @Override
//                public void run() {
//                    pbar.incrementProgressBy(1);
//                }
//            });
//        }
//
//        send(ArduinoCommands.CMD_SAVE);
//    }


    private String send(String cmd) throws Exception {
        log("Snd: " + cmd);
        String reply = bt.send(cmd);
        log("Rcv: " + reply);
        return reply;
    }


    public void disconnect() throws Exception {
        log("Disconnecting");

        if(!bt.isConnected()) {
            log("Not connected");
            return;
        }

//        bt.send(ArduinoCommands.CMD_EXIT);
        bt.close();
        log("Disconnected");
    }


    private void parse_config_reply(String reply) {
        for(String entry : reply.split(" ")) {
            String[] kv = entry.split(":", 2);
            Log.d("parse-config-reply", kv[0] + " => " + kv[1]);
        }
    }
}
