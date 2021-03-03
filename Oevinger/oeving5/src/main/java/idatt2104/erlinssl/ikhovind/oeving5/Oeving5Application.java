package idatt2104.erlinssl.ikhovind.oeving5;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.builder.SpringApplicationBuilder;
import org.springframework.boot.web.servlet.support.SpringBootServletInitializer;

@SpringBootApplication
public class Oeving5Application extends SpringBootServletInitializer {

	@Override
	protected SpringApplicationBuilder configure(SpringApplicationBuilder application) {
		return application.sources(Oeving5Application.class);

	}


	public static void main(String[] args) {
		SpringApplication.run(Oeving5Application.class, args);
	}

}
