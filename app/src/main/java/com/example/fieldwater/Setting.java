package com.example.fieldwater;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;

public class Setting extends AppCompatActivity {
    private DatabaseReference databaseReference= FirebaseDatabase.getInstance().getReference();

    private EditText d1,d2,d3,d4;
    @SuppressLint("MissingInflatedId")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);
        d1=findViewById(R.id.date1Et);
        d2=findViewById(R.id.date2Et);
        d3=findViewById(R.id.date3Et);
        d4=findViewById(R.id.date4Et);
    }

    public void saveDayClick(View view) {
        int dd1,dd2,dd3,dd4;
        dd1=dd2=dd3=dd4=0;

        try {

            dd1=Integer.parseInt(d1.getText().toString());
            dd2=Integer.parseInt(d2.getText().toString());
            dd3=Integer.parseInt(d3.getText().toString());
            dd4=Integer.parseInt(d4.getText().toString());
            databaseReference.child("Setting").setValue(dd1+"-"+dd2+"-"+dd3+"-"+dd4).addOnSuccessListener(runnable -> {
                Toast.makeText(getApplicationContext(),"Data added!",Toast.LENGTH_SHORT).show();
            });
        }catch (Exception e){
            Toast.makeText(getApplicationContext(),"Invalid data!",Toast.LENGTH_SHORT).show();
        }




    }
}