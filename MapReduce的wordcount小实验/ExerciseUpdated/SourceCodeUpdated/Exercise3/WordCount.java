/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.apache.hadoop.examples;

import java.io.IOException;
//import java.util.StringTokenizer;


import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.util.GenericOptionsParser;

public class WordCount {

	public static String age (String n) {
		int i = Integer.valueOf(n).intValue();
		switch (i/10) {
		case 0: return " 0-9";
		case 1: return " 10-19";
		case 2: return " 20-29";
		case 3: return " 30-39";
		case 4: return " 40-49";
		case 5: return " 50-59";
		case 6: return " 60-69";
		case 7: return " 70-79";
		default: return " More than 80 or unnormal age";
		}
	}
	
	public static String workhour (String n) {
		n = n.trim();
		int i = Integer.valueOf(n).intValue();
		switch (i/10) {
		case 0: return " 0-9";
		case 1: return " 10-19";
		case 2: return " 20-29";
		case 3: return " 30-39";
		case 4: return " 40-49";
		case 5: return " 50-59";
		case 6: return " 60-69";
		case 7: return " 70-79";
		default: return " More than 80 or unnormal workhour";
		}
	}
	
	public static String gainorloss (String n) {
		n = n.trim();
		int i = Integer.valueOf(n).intValue();
		switch (i) {
		case 0: return " 0";
		default: return " Not 0";
		}
	}
	
	public static String columnname (int n) {
		switch (n) {
		case 0: return "(AGE)";
		case 1: return "(WORKCLASS)";
		case 2: return "(FNLWGT)";
		case 3: return "(EDUCATION)";
		case 4: return "(EDUCATION-NUM)";
		case 5: return "(MARITAL-STATUS)";
		case 6: return "(OCCUPATION)";
		case 7: return "(RELATIONSHIP)";
		case 8: return "(RACE)";
		case 9: return "(SEX)";
		case 10: return "(CAPITAL-GAIN)";
		case 11: return "(CAPITAL-LOSS)";
		case 12: return "(HOURS-PER-WEEK)";
		case 13: return "(NATIVE-COUNTY)";
		default: return "unnormal column information";
		}
	}
	
  public static class TokenizerMapper 
       extends Mapper<Object, Text, Text, IntWritable>{
	  private final static IntWritable one = new IntWritable(1);
	    private Text word = new Text();
	    private String words = new String();
    
      
    public void map(Object key, Text value, Context context
                    ) throws IOException, InterruptedException {
      String[] ggroup = value.toString().split(",");
      for ( int i = 0; i<14 ; i++ ){
    	  if ((i != 0) && (i != 2) && (i != 10) && (i !=11) && (i != 12) && (i != 13)) {
    		   words = "FOR NATION" + ggroup[13] + ", ANALYZING " + columnname(i) + " THE NUMBER OF" + ggroup[i];
    		      word.set(words);
    		     context.write(word, one);
    	  }
    	  else if (i == 0) {
    		  words = "FOR NATION" + ggroup[13] + ", ANALYZING " + columnname(i) + " THE NUMBER OF" + age(ggroup[i]);
		      word.set(words);
		     context.write(word, one);
    	  }
    	  else if (i == 12) {
    		  words = "FOR NATION" + ggroup[13] + ", ANALYZING " + columnname(i) + " THE NUMBER OF" + workhour(ggroup[i]);
		      word.set(words);
		     context.write(word, one);
    	  }
    	  else if ((i == 10) || (i == 11)) {
    		  words = "FOR NATION" + ggroup[13] + ", ANALYZING " + columnname(i) + " THE NUMBER OF" + gainorloss(ggroup[i]);
		      word.set(words);
		     context.write(word, one);
    	  }
       }
    }
    }
  
  
  
  public static class IntSumReducer 
       extends Reducer<Text,IntWritable,Text,IntWritable> {
	  private IntWritable result = new IntWritable();

    public void reduce(Text key, Iterable<IntWritable> values, 
                       Context context
                       ) throws IOException, InterruptedException {
    	int sum = 0;
        for (IntWritable val : values) {
          sum += val.get();
        }
        result.set(sum);
        context.write(key, result);
    }
  }

  public static void main(String[] args) throws Exception {
    Configuration conf = new Configuration();
    String[] otherArgs = new GenericOptionsParser(conf, args).getRemainingArgs();
    
   // if (otherArgs.length != 3) {
   //   System.err.println("Usage: wordcount <in> <out>");
    //  System.exit(2);
   // }
    Job job = new Job(conf, "word count");
    job.setJarByClass(WordCount.class);
    job.setMapperClass(TokenizerMapper.class);
    job.setNumReduceTasks(3);
    //job.setCombinerClass(IntSumReducer.class);
    job.setReducerClass(IntSumReducer.class);
    job.setOutputKeyClass(Text.class);
    job.setOutputValueClass(IntWritable.class);
    FileInputFormat.addInputPath(job, new Path(otherArgs[0]));
    FileOutputFormat.setOutputPath(job, new Path(otherArgs[1]));
    System.exit(job.waitForCompletion(true) ? 0 : 1);
  }

}
