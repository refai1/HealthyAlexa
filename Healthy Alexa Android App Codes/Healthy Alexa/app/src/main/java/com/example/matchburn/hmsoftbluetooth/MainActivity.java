package com.example.matchburn.hmsoftbluetooth;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import android.content.Intent;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

/*
    Made by: Francisco Ramirez
    Get started screen
 */
public class MainActivity extends AppCompatActivity {

    View view;

    protected Button myButton;
    String myString = "Checking permissions...";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        view = this.getWindow().getDecorView();
        view.setBackgroundResource(R.color.Blue);

        myButton= (Button) findViewById(R.id.buttonId);
        myButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                Intent secondPage = new Intent(MainActivity.this,Main2Activity.class);
                startActivity(secondPage);


                Toast.makeText(getApplicationContext(),myString,Toast.LENGTH_SHORT).show();
            }
        });

    }
}
