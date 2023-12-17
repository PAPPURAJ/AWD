package com.example.fieldwater;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;

import com.google.firebase.database.ChildEventListener;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.jjoe64.graphview.DefaultLabelFormatter;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

public class GraphActivity extends AppCompatActivity {

    private GraphView graphView;
    private DatabaseReference databaseReference= FirebaseDatabase.getInstance().getReference();

    LineGraphSeries<DataPoint> series = new LineGraphSeries<>();
    int i=0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_graph);
        graphView=findViewById(R.id.idGraphView);

        // Set up the graph
        graphView.setTitle("My Graph View");
        graphView.setTitleColor(R.color.black);
        graphView.setTitleTextSize(18);
        graphView.addSeries(series);
        graphView.getViewport().setScalable(true);
        graphView.getViewport().setScalableY(true);
        graphView.getViewport().setYAxisBoundsManual(true);
        graphView.getViewport().setMinX(0);
        graphView.getViewport().setMaxX(4); // Adjust this value as needed
        graphView.getGridLabelRenderer().setLabelFormatter(new DefaultLabelFormatter() {
            @Override
            public String formatLabel(double value, boolean isValueX) {
                if (!isValueX) {
                    return String.format("%.0" +
                            "fcm", value);
                }
                return super.formatLabel(value, isValueX);
            }
        });

        loadData();
    }

    void loadData(){
        databaseReference.child("WaterLevel").addChildEventListener(new ChildEventListener() {
            @Override
            public void onChildAdded(@NonNull DataSnapshot snapshot, @Nullable String previousChildName) {
                try{
                    series.appendData(new DataPoint(i++, Double.parseDouble(snapshot.getValue(String.class))),true,i);
                    graphView.addSeries(series);
                    series.notify();
                    graphView.notify();
                    graphView.invalidate();
                    graphView.computeScroll();
                }catch (Exception e){
                    Log.e("GraphActivity", "Error updating graph", e);
                }
            }

            @Override
            public void onChildChanged(@NonNull DataSnapshot snapshot, @Nullable String previousChildName) {

            }

            @Override
            public void onChildRemoved(@NonNull DataSnapshot snapshot) {

            }

            @Override
            public void onChildMoved(@NonNull DataSnapshot snapshot, @Nullable String previousChildName) {

            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }

        });

    }


}