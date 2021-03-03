package idatt2104.erlinssl.ikhovind.oeving5.web;

import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.*;
import java.io.*;
import java.net.URI;
import java.net.URLDecoder;
import java.nio.charset.StandardCharsets;

@Controller
public class Oeving5Controller {

    @RequestMapping("/")
    public String welcome() {
        return "index";
    }

    @PostMapping("/programs")
    public ResponseEntity<String> enterCode(@RequestBody String input) throws IOException, InterruptedException {
        String unEscapedHTML = URLDecoder.decode(input, StandardCharsets.UTF_8);
        if(unEscapedHTML.contains("inputCode=")) unEscapedHTML = unEscapedHTML.substring("inputCode=".length());
        BufferedWriter writer = new BufferedWriter(new FileWriter("docker/out.cpp"));
        writer.write(unEscapedHTML);
        writer.close();

        Process buildImage = Runtime.getRuntime().exec("docker build . -t temp", null, new File("docker/"));
        int exitcode = buildImage.waitFor();

        if(exitcode == 0) {
            Process runImage = Runtime.getRuntime().exec("docker run temp");
            runImage.waitFor();
            String codeOutput = new String(runImage.getInputStream().readAllBytes());

            Runtime.getRuntime().exec("docker image rm temp");

            return new ResponseEntity<String>(codeOutput, HttpStatus.OK);
        }
        //exitcode other than 0 means an error (code didn't compile successfully probably)
        else{
            //TODO return an error message to output

            //todo figure out what error to use here
            return new ResponseEntity("compilation error", HttpStatus.BAD_REQUEST);

        }
    }
}