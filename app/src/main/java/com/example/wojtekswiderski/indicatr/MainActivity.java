package com.example.wojtekswiderski.indicatr;

import android.app.Activity;
import android.bluetooth.*;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ListView;
import android.widget.TextView;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends Activity {

    TextView connection_view;
    TextView direction_view;
    Button on_button;
    Button off_button;
    String received;
    CheckBox left_hand;

    private final static UUID PEBBLE_APP_UUID = UUID.fromString("78a84721-0404-4b4f-b91b-1a3fdf41e86c");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        connection_view = (TextView) findViewById(R.id.connection_view);
        direction_view = (TextView) findViewById(R.id.direction_view);
        on_button = (Button) findViewById(R.id.on_button);
        off_button = (Button) findViewById(R.id.off_button);
        left_hand = (CheckBox) findViewById(R.id.left_hand);

        boolean connected = PebbleKit.isWatchConnected(getApplicationContext());
        if(connected){
            connection_view.setText(String.format("Connected"));
        }else{
            connection_view.setText(String.format("Not Connected"));
        }

        PebbleKit.registerPebbleConnectedReceiver(getApplicationContext(), new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                connection_view.setText(String.format("Connected"));
            }
        });

        PebbleKit.registerPebbleDisconnectedReceiver(getApplicationContext(), new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                connection_view.setText(String.format("Not Connected"));
            }
        });

        PebbleKit.registerReceivedDataHandler(this, new PebbleKit.PebbleDataReceiver(PEBBLE_APP_UUID) {
            @Override
            public void receiveData(final Context context, final int transactionId, final PebbleDictionary data) {
                received = data.getString(1);
                if(left_hand.isChecked()) {
                    if (received.equals("1")) {
                        direction_view.setText("Turning Right...");
                    } else if (received.equals("2")) {
                        direction_view.setText("Turning Left...");
                    } else if (received.equals("3")) {
                        direction_view.setText("Braking...");
                    } else {
                        direction_view.setText("Biking...");
                    }
                }else{
                    if (received.equals("1")) {
                        direction_view.setText("Turning Left...");
                    } else if (received.equals("2")) {
                        direction_view.setText("Braking...");
                    } else if (received.equals("3")) {
                        direction_view.setText("Turning Right...");
                    } else {
                        direction_view.setText("Biking...");
                    }
                }
                PebbleKit.sendAckToPebble(getApplicationContext(), transactionId);
            }
        });

    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    public void open_app(View view){
        PebbleKit.startAppOnPebble(getApplicationContext(), PEBBLE_APP_UUID);
        direction_view.setText("Biking");
    }

    public void close_app(View view){
        PebbleKit.closeAppOnPebble(getApplicationContext(), PEBBLE_APP_UUID);
        direction_view.setText("Not Connected...");
    }

}
