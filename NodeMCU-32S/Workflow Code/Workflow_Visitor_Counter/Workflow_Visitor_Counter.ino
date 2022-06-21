function main(parameter){
    output=[];
    /* Start your code here, you can access parameter values as normal array starting from index 1 e.g. var i = parameter[1], you can write to output value as normal array starting from index 1 e.g. output[1]= 1+1;*/
    var obj = parameter[1]["data"];
    var arrlgt = parameter[1]["count"].total - 1;
    obj.people = obj[arrlgt]["Obstacle"];
    threshold = 10;
    
        if(obj.people > threshold){
            msgbody='<p>Number of customers has reached the treshold. Kindly close the gate.</p><br>';
            output[1]="[Alert] Close the Gate ";//to subject
            output[2]=msgbody;//to template
            output[3]=2;//to data
            
    /* end your code here*/
        }
    
    
    return output;
}
